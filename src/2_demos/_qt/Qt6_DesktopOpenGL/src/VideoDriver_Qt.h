#pragma once

class VideoDriver_QtPrivate;

class VideoDriver_Qt 
{
public:
    VideoDriver_Qt();
    ~VideoDriver_Qt();

    void init();
	void deinit();
    void render();

    int w() const;
    int h() const;
    void setScreenSize(int w, int h);
private:
    VideoDriver_QtPrivate* m_private;
};
