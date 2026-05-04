#pragma once
#include <QWidget>

class GL_Widget_WGL_Impl;

class GL_Widget_WGL : public QWidget
{
    Q_OBJECT
public:
    GL_Widget_WGL(QWidget* parent = nullptr);
    ~GL_Widget_WGL() override;

protected:
    virtual void initializeGL() = 0;
    virtual void resizeGL(int w, int h) = 0;
    virtual void paintGL() = 0;


    QPaintEngine* paintEngine() const override
    {
        return nullptr; // no QPainter, no backing store
    }

    void showEvent(QShowEvent* e) override;
    void paintEvent(QPaintEvent*) override;
    void resizeEvent(QResizeEvent* e) override;
    // bool nativeEvent(const QByteArray&, void* message, long* result) override;

private:
    void initGL();
    void destroyGL();

private:
    GL_Widget_WGL_Impl* _d;
};
