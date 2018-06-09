#pragma once

namespace Interop {
namespace AVFoundation {

struct _AVCaptureSession;

class AVCaptureSession
{

public:
	AVCaptureSession();
	virtual ~AVCaptureSession();

private:
	_AVCaptureSession* _wrapped;

};

}
}
