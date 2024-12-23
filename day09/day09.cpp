#include "day09.h"

#include "parsers.h"

#include <algorithm>
#include <exception>
#include <fstream>
#include <iostream>
#include <memory>
#include <numeric>
#include <ranges>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;

namespace day09
{

namespace
{

class Block
{
    int id_;

public:
    Block() : id_(-1) {} // default to empty space; -1 means empty space
    Block(int id) : id_(id) {}

    bool is_space() const
    {
        return id_ == -1;
    }

    int id() const
    {
        return id_;
    }

    void set_id(int id)
    {
        id_ = id;
    }
};

using Blocks = vector<Block>;

class HardDrive
{
    Blocks blocks_;

public:
    HardDrive(Blocks&& files_and_space)
        : blocks_(std::move(files_and_space))
    {
        if (blocks_.empty())
        {
            throw invalid_argument{"Empty hard drive"};
        }
    }

    HardDrive(const HardDrive&) = default;
    HardDrive(HardDrive&&) noexcept = default;

    HardDrive& operator=(const HardDrive&) = default;
    HardDrive& operator=(HardDrive&&) noexcept = default;

    void compact()
    {
        size_t s = 0;
        size_t f = blocks_.size() - 1;

        while (s < f)
        {
            while (s < blocks_.size() && !blocks_[s].is_space())
            {
                ++s;
            }

            while (f > s && blocks_[f].is_space())
            {
                --f;
            }

            if (s < f)
            {
                swap(blocks_[s], blocks_[f]);
            }
        }
    }

    void defragment()
    {
        unordered_set<int> moved_files;

        for (size_t f = blocks_.size() - 1; f < blocks_.size(); --f)
        {
            while (blocks_[f].is_space() || moved_files.find(blocks_[f].id()) != moved_files.end())
            {
                --f;
            }

            if (f == numeric_limits<size_t>::max())
            {
                break;
            }

            // at this point, f is at the (rightmost) edge of a file.  count how big the file is.
            int cur_id = blocks_[f].id();
            size_t block_size = 0;
            while (f != numeric_limits<size_t>::max() && blocks_[f].id() == cur_id)
            {
                --f;
                ++block_size;
            }

            // f is no longer pointed at the last block of the file; it's now pointed at the first block of the next file.
            ++f;

            // CONDITION: block_size >= 1

            // now we know how big the file is; find the left-most block of spaces that will fit it.
            for (size_t s = 0; s < f; s++)
            {
                size_t found = 0;
                while (s + found < f && blocks_[s + found].is_space() && found < block_size)
                {
                    ++found;
                }

                if (found == block_size)
                {
                    // we found a contiguous block of spaces that will fit the file.  move the file there.
                    for (size_t i = 0; i < block_size; ++i)
                    {
                        blocks_[s + i].set_id(cur_id);
                        blocks_[f + i].set_id(-1);
                    }

                    break;
                }

                // skip any spaces we found (the for-loop will move s past them)
                s += found;
            }

            moved_files.insert(cur_id);
        }
    }

    uintmax_t checksum() const
    {
        uintmax_t sum = 0;
        for (size_t i = 0; i < blocks_.size(); ++i)
        {
            // logging is slow, so only do it when we're debugging at -vvv
            dbg(LogLevel::TRACE) << " i=" << i << " block=" << blocks_[i].id() << " sum=" << sum << endl;

            if (blocks_[i].is_space())
            {
                continue;
            }

            size_t mul = static_cast<size_t>(blocks_[i].id()) * i;
            if (sum + mul < sum)
            {
                throw overflow_error{"Overflow"};
            }

            sum += mul;
        }
        return sum;
    }
};

HardDrive make_from_repr(const string& repr)
{
    // Format:
    // repr is a "compact map" of alternating file and space blog segments.
    // The first charactar is a number of contiguous blocks representing one file.
    // The next character is a number of contiguous blocks representing empty space.
    // Then comes files blocks, then space blocks, and so on.
    //
    // Files have IDs starting at 0, and are numbered in the order in which they appear
    // in the compact map (so map[0] is file ID 0, map[2] is file ID 1, etc).
    //
    // PRECONDITION: No contiguous group of file or space blocks with more than 9 blocks.
    // PRECONDITION: Files are contiguous (that is, not fragmented).

    size_t num_blocks = accumulate(
        repr.begin(), repr.end(),
        0_z,
        [](size_t acc, char c) { return acc + static_cast<size_t>(c - '0'); }
    );

    Blocks blocks(num_blocks);

    int file_id = 0;
    size_t it = 0;

    for (size_t i = 0; i < repr.size(); i += 2)
    {
        for (int n = repr[i] - '0'; n > 0; --n)
        {
            blocks[it++].set_id(file_id);
        }

        // adjust for spaces
        it += static_cast<size_t>(repr[i + 1] - '0');

        file_id++;
    }

    return HardDrive{std::move(blocks)};
}

constexpr const char* kInputFile = "day09/day09.input";
constexpr const char* kTestInput = "2333133121414131402";

unique_ptr<istream> get_input()
{
    if (g_test_input)
    {
        return make_unique<stringstream>(kTestInput);
    }

    return make_unique<ifstream>(kInputFile);
}

HardDrive read_input()
{
    auto input = get_input();
    string line = parsers::String(*input);
    return make_from_repr(line);
}

} // namespace

string PartOne::solve()
{
    HardDrive drive = read_input();
    drive.compact();
    return to_string(drive.checksum());
}

string PartTwo::solve()
{
    HardDrive drive = read_input();
    drive.defragment();
    return to_string(drive.checksum());
}

} // namespace day09
