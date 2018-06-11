#include "constants.hpp"
#include "parsers/MediaParser.hpp"
#include <memory>

class Media;

class VideoParser : public MediaParser
{
public:
    VideoParser(const xlf_node& parent_node, const xlf_node& media_node);

protected:
    std::unique_ptr<Media> doParse() override;

};
