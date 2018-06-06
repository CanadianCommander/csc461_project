#ifndef NSCREEN_H_
#define NSCREEN_H_

#include <memory>
#include <cstdint>
#include "Texture.h"

//GTK 2 forward defs
#ifdef __linux__
  typedef struct _GdkWindow GdkWindow;
  typedef struct _GdkPixbuf GdkPixbuf;
#endif

namespace nscreen{

  class Screen {
  public:
    Screen();
    virtual ~Screen();

    std::shared_ptr<Texture> getScreenFB();

  protected:
    std::shared_ptr<Texture> getScreenPixelBuffer();
    void queryDisplayInformation();

  private:
    #ifdef __linux__
      GdkWindow * mDesktop;
    #elif __APPLE__
      uint32_t mDesktop;
    #endif
    int mDesktopX;
    int mDesktopY;
    int mDesktopWidth;
    int mDesktopHeight;
  };


}


#endif /*NSCREEN_H_*/
