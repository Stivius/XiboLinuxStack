#include "FlyTransitionExecutor.hpp"

#include "common/logger/Logging.hpp"

FlyTransitionExecutor::FlyTransitionExecutor(Transition::Heading heading,
                                             Transition::Direction direction,
                                             int duration,
                                             const std::shared_ptr<Xibo::Widget>& media) :
    TransitionExecutor(heading, duration, media),
    direction_(direction)
{
}

void FlyTransitionExecutor::apply()
{
    std::string h = heading() == Transition::Heading::In ? "in" : "out";

    Log::debug("Executing fly {} transition with {} duration", h, duration());
}