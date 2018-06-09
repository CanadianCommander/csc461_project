#import <AVFoundation/AVCaptureSession.h>

using AVCaptureSessionObjC = AVCaptureSession;

#import "AVCaptureSession.h"

namespace Interop {
namespace AVFoundation {

struct _AVCaptureSession
{
	AVCaptureSessionObjC* Object;
};

AVCaptureSession::AVCaptureSession() : _wrapped(new _AVCaptureSession())
{
	auto object = _wrapped->Object = [[AVCaptureSessionObjC alloc] init];
	object.sessionPreset = AVCaptureSessionPresetPhoto;
}

AVCaptureSession::~AVCaptureSession()
{
	if (_wrapped->Object)
	{
		[_wrapped->Object release];
	}

	delete _wrapped;
}

}
}