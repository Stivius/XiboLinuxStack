#include "Media.hpp"

class FilePath;
class IAudioHandler;

class Audio : public Media<IInvisibleMedia>
{
public:
    Audio(int id, const FilePath& path, std::unique_ptr<IAudioHandler>&& handler);
    void apply(MediaVisitor& visitor) override;

    void setVolume(int volume);
    void setLooped(bool looped);

protected:
    void doStop() override;
    void doStart() override;
    void onMediaTimeout() override;

private:
    void onAudioFinished();

private:
    std::unique_ptr<IAudioHandler> m_handler;
    bool m_looped = false;

};
