#include "ScheduleParser.hpp"

#include "common/Parsing.hpp"
#include "common/dt/DateTime.hpp"
#include "common/fs/FilePath.hpp"
#include "common/fs/FileSystem.hpp"
#include "networking/xmds/Resources.hpp"

#include <boost/property_tree/xml_parser.hpp>

namespace Resources = XmdsResources::Schedule;

const char* ScheduleParseException::what() const noexcept
{
    return "Schedule is invalid";
}

LayoutSchedule ScheduleParser::scheduleFrom(const FilePath& path)
{
    try
    {
        if (!FileSystem::exists(path)) return {};

        return scheduleFromImpl(Parsing::xmlFromPath(path));
    }
    catch (std::exception&)
    {
        throw ScheduleParseException{};
    }
}

LayoutSchedule ScheduleParser::scheduleFrom(const std::string& xmlSchedule)
{
    try
    {
        return scheduleFromImpl(Parsing::xmlFromString(xmlSchedule));
    }
    catch (std::exception&)
    {
        throw ScheduleParseException{};
    }
}

LayoutSchedule ScheduleParser::scheduleFromImpl(const ptree_node& scheduleXml)
{
    LayoutSchedule schedule;
    auto scheduleNode = scheduleXml.get_child(Resources::Schedule);

    schedule.generatedTime = DateTime::fromString(scheduleNode.get<std::string>(Resources::Generated));

    for (auto [name, node] : scheduleNode)
    {
        if (name == Resources::Layout)
            schedule.regularLayouts.emplace_back(scheduledLayoutFrom(node));
        else if (name == Resources::DefaultLayout)
            schedule.defaultLayout = defaultLayoutFrom(node);
        else if (name == Resources::Overlays)
            schedule.overlayLayouts = overlayLayoutsFrom(node);
        else if (name == Resources::GlobalDependants)
            schedule.globalDependants = dependantsFrom(node);
    }

    return schedule;
}

ScheduledLayout ScheduleParser::scheduledLayoutFrom(const ptree_node& node)
{
    ScheduledLayout layout;

    layout.scheduleId = node.get<int>(Resources::ScheduleId);
    layout.id = node.get<int>(Resources::Id);
    layout.startDT = DateTime::fromString(node.get<std::string>(Resources::StartDT));
    layout.endDT = DateTime::fromString(node.get<std::string>(Resources::EndDT));
    layout.priority = node.get<int>(Resources::Priority);

    if (auto dependants = node.get_child_optional(Resources::LocalDependants))
    {
        layout.dependants = dependantsFrom(dependants.value());
    }

    return layout;
}

DefaultScheduledLayout ScheduleParser::defaultLayoutFrom(const ptree_node& node)
{
    DefaultScheduledLayout layout;

    layout.id = node.get<int>(Resources::Id);
    if (auto dependants = node.get_child_optional(Resources::LocalDependants))
    {
        layout.dependants = dependantsFrom(dependants.value());
    }

    return layout;
}

LayoutList ScheduleParser::overlayLayoutsFrom(const ptree_node& overlaysNode)
{
    LayoutList overlayLayouts;

    for (auto [name, node] : overlaysNode)
    {
        overlayLayouts.emplace_back(scheduledLayoutFrom(node));
    }

    return overlayLayouts;
}

LayoutDependants ScheduleParser::dependantsFrom(const ptree_node& dependantsNode)
{
    LayoutDependants dependants;

    for (auto&& [name, file] : dependantsNode)
    {
        dependants.emplace_back(file.get_value<std::string>());
    }

    return dependants;
}
