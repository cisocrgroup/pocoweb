#ifndef pcw_Pix_hpp__
#define pcw_Pix_hpp__

#include <leptonica/allheaders.h>
#include <memory>

namespace pcw {
struct PixDestroy
{
  void operator()(PIX* pix) const noexcept { pixDestroy(&pix); }
};
using PixPtr = std::unique_ptr<PIX, PixDestroy>;
}

#endif // pcw_Pix_hpp__
