#include "dawg.h"

#include <algorithm>
#include <array>
#include <numeric>
#include <unordered_map>
#include <unordered_set>

using namespace std;

namespace
{

class Trie : public enable_shared_from_this<Trie>
{
public:
    using Ptr = shared_ptr<Trie>;

    Trie()
        : children_{}
        , eow_{false}
        , hash_{numeric_limits<size_t>::max()}
        , height_{numeric_limits<size_t>::max()}
    {}

    const Ptr& at(char c) const
    {
        return children_[tolower(c) - 'a'];
    }

    Ptr& at(char c)
    {
        return children_[tolower(c) - 'a'];
    }

    auto children_begin() const
    {
        return children_.begin();
    }

    auto children_end() const
    {
        return children_.end();
    }

    bool eow() const
    {
        return eow_;
    }

    void add(const string& word)
    {
        auto n = shared_from_this();
        for (size_t i = 0; i < word.size(); ++i)
        {
            auto c = tolower(word[i]) - 'a';
            if (!n->children_[c])
            {
                n->children_[c] = make_shared<Trie>();
            }
            n = n->children_[c];
        }
        n->eow_ = true;
    }

    size_t hash()
    {
        if (hash_ == numeric_limits<size_t>::max())
        {
            // We've got a Pretty Good Hash Code here.
            // Our alphabet is 26 characters, so we can use 26 bits to represent
            // our set of children perfectly.  That leaves us (sizeof(size_t) * 8) - 26
            // bits to store the height of the tree.

            hash_ = 0;
            for (size_t i = 0; i < 26; ++i)
            {
                if (children_[i])
                {
                    hash_ |= (1 << i);
                }
            }

            int shift = (sizeof(hash_) * 8) - 26;
            hash_ <<= shift;

            int mask = (1 << shift) - 1;
            hash_ |= mask & height();
        }
        return hash_;
    }

    size_t height()
    {
        if (height_ == numeric_limits<size_t>::max())
        {
            height_ = 1 + transform_reduce(
                children_.begin(), children_.end(),
                static_cast<size_t>(0),
                [](size_t a, size_t b) { return max(a, b); },
                [](const Ptr& p) { return p ? p->height() : 0; }
            );
        }
        return height_;
    }

private:
    array<Ptr, 26> children_;
    bool eow_;
    size_t hash_;
    size_t height_;
};

struct TrieHash
{
    size_t operator()(const Trie::Ptr& trie) const
    {
        return trie->hash();
    }
};

struct CanonicalEquality
{
    bool operator()(const Trie::Ptr& a, const Trie::Ptr& b) const
    {
        if (a == nullptr && b == nullptr)
        {
            return true;
        }

        if (a == nullptr || b == nullptr)
        {
            return false;
        }

        if (a->eow() != b->eow())
        {
            return false;
        }

        if (a->height() != b->height())
        {
            return false;
        }

        return equal(a->children_begin(), a->children_end(), b->children_begin());
    }
};

using CanonicalSet = std::unordered_set<Trie::Ptr, TrieHash, CanonicalEquality>;

class DawgBuilder
{
    Trie::Ptr root_;
    CanonicalSet canon_;
    vector<uint16_t> dawg_;
    vector<Trie::Ptr> num_to_node_;
    unordered_map<Trie::Ptr, size_t, TrieHash> node_to_num_;

public:
    DawgBuilder()
        : root_{make_shared<Trie>()}
        , canon_{}
    {}

    void add(const string& word)
    {
        root_->add(word);
    }

    unique_ptr<Dawg> build()
    {
        canonicalize();
        num_to_node_.reserve(canon_.size() + 1);
        num_to_node_.push_back(nullptr);
        copy(canon_.begin(), canon_.end(), back_inserter(num_to_node_));

        for (size_t ix = 1; ix < num_to_node_.size(); ++ix)
        {
            node_to_num_[num_to_node_[ix]] = ix;
        }

        dawg_.resize(27 * canon_.size(), 0);

        fill_dawg(root_);

        return make_unique<Dawg>(std::move(dawg_), node_to_num_[root_]);
    }

private:
    void canonicalize()
    {
        // This process converts our trie into a true DAWG by sharing
        // common suffixes.
        for (size_t height = 1; height < root_->height(); ++height)
        {
            canonicalize_at_height(root_, height);
        }
    }

    void canonicalize_at_height(const Trie::Ptr& node, size_t height)
    {
        if (node == nullptr)
        {
            return;
        }

        if (node->height() > height)
        {
            for (auto it = node->children_begin(); it != node->children_end(); ++it)
            {
                canonicalize_at_height(*it, height);
            }
        }
        else if (node->height() == height)
        {
            for (size_t ix = 0; ix < 26; ++ix)
            {
                auto c = node->at(ix);
                auto it = canon_.find(c);
                if (it != canon_.end())
                {
                    node->at(ix) = *it;
                }
                else
                {
                    canon_.insert(c);
                }
            }
        }
    }

    void fill_dawg(const Trie::Ptr& node)
    {
        if (node == nullptr)
        {
            return;
        }

        for (size_t ix = 0; ix < 26; ++ix)
        {
            auto c = node->at(ix);
            if (c)
            {
                fill_dawg(c);
            }
        }

        auto num = node_to_num_[node];
        for (size_t ix = 0; ix < 26; ++ix)
        {
            auto c = node->at(ix);
            if (c != nullptr)
            {
                dawg_[num * 27 + ix] = node_to_num_[c];
            }
        }

        if (node->eow())
        {
            dawg_[num * 27 + 26] = 1;
        }
    }
};

} // namespace

Dawg::Dawg(vector<uint16_t>&& trie, uint16_t root)
    : trie_(std::move(trie))
    , root_(root)
{}

bool Dawg::contains(const string& word) const
{
    uint16_t node = root_;
    for (char c : word)
    {
        node = next(node, c);
        if (node == 0)
        {
            return false;
        }
    }
    return eow(node);
}

uint16_t Dawg::root() const
{
    return root_;
}

bool Dawg::eow(uint16_t node) const
{
    return trie_[node * 27 + 26] == 1;
}

uint16_t Dawg::next(uint16_t node, char c) const
{
    return trie_[node * 27 + tolower(c) - 'a'];
}

unique_ptr<Dawg> BuildDawg(const vector<string>& words)
{
    DawgBuilder builder;
    for (const auto& word : words)
    {
        builder.add(word);
    }
    return builder.build();
}
