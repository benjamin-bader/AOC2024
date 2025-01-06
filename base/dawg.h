#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

class IWordSet
{
public:
    virtual ~IWordSet() = default;
    virtual bool contains(const std::string& word) const = 0;
};

/**
 * @brief A Directed Acyclic Word Graph (DAWG) implementation.
 *
 * It is specialized for the English lowercase alphabet only - no spaces,
 * numbers, or punctuation.
 *
 * Representation here is a 2D array of 16-bit integers.  Each row is 27
 * elements wide, and represents one conceptual node of the graph.  The
 * first 26 elements identify the successor node for the given character,
 * and the final element is an end-of-word bit.
 *
 * It implements IWordSet for easy use, but also exposes a lower-level API
 * for working directly with nodes.
 *
 * This is more expensive to build than a Trie or a set, but has great memory
 * locality and is very fast to query.
 */
class Dawg : public IWordSet
{
    std::vector<std::uint16_t> trie_;
    std::uint16_t root_;

public:
    Dawg(std::vector<std::uint16_t>&& trie, std::uint16_t root);
    virtual ~Dawg() = default;

    // High-level interface
    bool contains(const std::string& word) const override;

    // Low-level interface

    /**
     * @brief Returns the root node of the graph - queries start here.
     *
     * @return std::uint16_t
     */
    std::uint16_t root() const;

    /**
     * @brief Returns true if the given node is an end-of-word node.
     */
    bool eow(std::uint16_t node) const;

    /**
     * @brief Returns the successor node for the given node and character.
     */
    std::uint16_t next(std::uint16_t node, char c) const;
};

std::unique_ptr<Dawg> BuildDawg(const std::vector<std::string>& words);
