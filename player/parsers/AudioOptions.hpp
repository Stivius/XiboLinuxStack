#pragma once

#include "MediaOptions.hpp"

namespace ResourcesXlf
{
    class AudioNodeTag { };

    class AudioOptions : public MediaOptions
    {
    public:
        AudioOptions(int id,
                     boost::optional<std::string> path,
                     int duration,
                     boost::optional<bool> muted,
                     boost::optional<bool> looped,
                     boost::optional<int> volume);
        AudioOptions(const xml_node& node);
        AudioOptions(const xml_node& node, AudioNodeTag);

        boost::optional<bool> muted() const;
        boost::optional<bool> looped() const;
        boost::optional<int> volume() const;

    private:
        boost::optional<bool> m_muted;
        boost::optional<bool> m_looped;
        boost::optional<int> m_volume;
    };
}
