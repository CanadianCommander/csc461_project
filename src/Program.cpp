#include "Debug.h"
#include "Program.h"
#include "Codec/Transcoders/Open264Transcoder.h"
#include "Codec/Transcoders/VPXTranscoder.h"
#include "Codec/Packets/VPXPacket.h"

#ifndef NDEBUG

Program::Program(bool isSender, std::string ipAddr, uint16_t portNum, LogPriority logPriority, LogCategory logCategory)
#else

Program::Program(bool isSender, std::string ipAddr, uint16_t portNum)
#endif
{
#ifndef NDEBUG
	SetLogPriority(logPriority);
	SetLogCategory(logCategory);
#endif
	_isSender = isSender;
	_ipAddr = ipAddr;
	_portNum = portNum;
	if (!_isSender)
	{
		InitializeSDL();
		InitializeOpenGL();
	}
	InitializeNetwork();
	_isExiting = false;
	_transcoder = std::make_shared<Codec::VPXTranscoder>();
	_transcoder->InitEncoder();
	_transcoder->InitDecoder();
}

Program::~Program()
{
	SDL_GL_DeleteContext(_context);
	SDL_DestroyWindow(_window);
	SDL_Quit();
}

void Program::InitializeSDL()
{
	SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS);
	LogSDL("SDL_Init");

	_windowWidth = 640;
	_windowHeight = 480;
	_window = SDL_CreateWindow(
			"CSC 461",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			_windowWidth,
			_windowHeight,
			SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE);
	LogSDL("SDL_CreateWindow");

	SDL_version sdlVersion;
	SDL_GetVersion(&sdlVersion);
	LogInfo(LogCategory::ALL, "SDL version: %d.%d.%d.", sdlVersion.major, sdlVersion.minor, sdlVersion.patch);
}

void Program::InitializeOpenGL()
{
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	LogSDL("SDL_GL_SetAttribute");
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	LogSDL("SDL_GL_SetAttribute");
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	LogSDL("SDL_GL_SetAttribute");
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	LogSDL("SDL_GL_SetAttribute");
	_context = SDL_GL_CreateContext(_window);
	LogSDL("SDL_GL_CreateContext");
	SDL_GL_SetSwapInterval(1);
	LogSDL("SDL_GL_SetSwapInterval");

	LogInfo(LogCategory::GRAPHICS, "OpenGL version: %s", glGetString(GL_VERSION));

	glGenVertexArrays(1, &_vertexArrayHandle);
	LogGL("glGenVertexArrays");
	glBindVertexArray(_vertexArrayHandle);
	LogGL("glBindVertexArray");

	glGenBuffers(1, &_vertexBufferHandle);
	LogGL("glGenBuffers");

	GLfloat vertices[] = {
			-1, 1, 0, 0, // Top-left
			1, 1, 1, 0, // Top-right
			1, -1, 1, 1, // Bottom-right
			-1, -1, 0, 1  // Bottom-left
	};
	glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferHandle);
	LogGL("glBindBuffer");
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	LogGL("glBufferData");

	/*
	*  TL    TR
	*   0----1
	*   |\   |
	*   | \  |
	*   |  \ |
	*   |   \|
	*   3----2
	*  BL    BR
	*/

	glGenBuffers(1, &_indexBufferHandle);
	LogGL("glGenBuffers");

	GLubyte indices[] = {
			0, 1, 2,
			2, 3, 0
	};

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferHandle);
	LogGL("glBindBuffer");
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	LogGL("glBufferData");

	const char* vertexShaderSource =

#include "Graphics/Shaders/textureShader.vs.glsl"
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	LogGL("glCreateShader");
	glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
	LogGL("glCompileSource");
	glCompileShader(vertexShader);
	LogGL("glCompileShader");

	auto fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	LogGL("glCreateShader");
	const char* fragmentShaderSource =

#include "Graphics/Shaders/textureShader.fs.glsl"
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
	LogGL("glCompileSource");
	glCompileShader(fragmentShader);
	LogGL("glCompileShader");

	auto shaderProgram = glCreateProgram();
	LogGL("glCompileProgram");
	glAttachShader(shaderProgram, vertexShader);
	LogGL("glAttachShader");
	glAttachShader(shaderProgram, fragmentShader);
	LogGL("glAttachShader");
	glLinkProgram(shaderProgram);
	LogGL("glLinkProgram");
	glUseProgram(shaderProgram);
	LogGL("glUseProgram");

	auto vertexPositionAttributeLocation = static_cast<GLuint>(glGetAttribLocation(shaderProgram, "vertexPosition"));
	LogGL("glGetAttribLocation");
	glEnableVertexAttribArray(vertexPositionAttributeLocation);
	LogGL("glEnableVertexAttribArray");
	glVertexAttribPointer(vertexPositionAttributeLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), nullptr);
	LogGL("glVertexAttribPointer");

	auto vertexTextureCoordinateLocation = static_cast<GLuint>(glGetAttribLocation(shaderProgram,
	                                                                               "vertexTextureCoordinate"));
	LogGL("glGetAttribLocation");
	glEnableVertexAttribArray(vertexTextureCoordinateLocation);
	LogGL("glEnableVertexAttribArray");
	glVertexAttribPointer(vertexTextureCoordinateLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat),
	                      (void*)(2 * sizeof(GLfloat)));
	LogGL("glVertexAttribPointer");

	glActiveTexture(GL_TEXTURE0);
	LogGL("glActiveTexture");

	std::shared_ptr<IO::Image> img = _screenCapture.GetScreenFrameBuffer();

	_texture = new Graphics::Texture(img->GetWidth(), img->GetHeight(), GL_RGB, GL_RGB, GL_UNSIGNED_BYTE);
	_texture->UploadData(img->GetRawDataPtr());

	auto textureSamplerUniformLocation = glGetUniformLocation(shaderProgram, "textureSampler");
	LogGL("glGetUniformLocation");
	glUniform1i(textureSamplerUniformLocation, 0);
	LogGL("glUniform1i");
}

void Program::InitializeNetwork()
{
	auto status = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	LogVerboseOrCritical(LogCategory::NETWORK, status >= 0, "socket");
	_socketHandle = (uint32_t)status;

	memset(&_socketAddress, 0, sizeof(_socketAddress));
	_socketAddress.sin_family = AF_INET;
	if (_isSender)
	{
		_socketAddress.sin_port = htons(8081);
		_socketAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	} else
	{
		_socketAddress.sin_port = htons(_portNum);
		_socketAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	}


	socklen_t wtf = sizeof(int);
	status = getsockopt(_socketHandle, SOL_SOCKET, SO_SNDBUF, &_socketSendBufferSize, &wtf);
	LogVerboseOrCritical(LogCategory::NETWORK, status != -1, "getsockopt");
	status = getsockopt(_socketHandle, SOL_SOCKET, SO_RCVBUF, &_socketReceiveBufferSize, &wtf);
	LogVerboseOrCritical(LogCategory::NETWORK, status != -1, "getsockopt");
	// see https://linux.die.net/man/7/socket (section on, SO_SNDBUF and SO_RCVBUF )
	_socketSendBufferSize = _socketSendBufferSize/8;
	_socketReceiveBufferSize = _socketReceiveBufferSize/8;

	if(!_isSender){
		status = bind(_socketHandle, (struct sockaddr*)&_socketAddress, sizeof(_socketAddress));
	}
	LogVerboseOrCritical(LogCategory::NETWORK, status != -1, "bind");

	memset(&_socketSendAddress, 0, sizeof(_socketSendAddress));
	_socketSendAddress.sin_family = AF_INET;
	_socketSendAddress.sin_port = htons(_portNum);
	status = inet_aton(_ipAddr.c_str(), &_socketAddress.sin_addr);
	LogVerboseOrCritical(LogCategory::NETWORK, status != 0, "inet_aton");

	if (!_isSender)
	{
		_socketReceiveThread = std::thread(SocketReceive, this);
	}
}

void Program::Loop()
{
	_previousFrameTimePoint = hclock::now();

	while (!_isExiting)
	{
		RetryTick:
		auto currentFrameTimePoint = hclock::now();
		_accumulatedFrameTime += std::chrono::duration_cast<millisecondsf>(
				currentFrameTimePoint - _previousFrameTimePoint);
		_previousFrameTimePoint = currentFrameTimePoint;

		if (_isFixedUpdate && _accumulatedFrameTime < _targetElapsedTime)
		{
			auto sleepDuration = _targetElapsedTime - _accumulatedFrameTime;
			std::this_thread::sleep_for(sleepDuration);
			goto RetryTick;
		}

		if (_accumulatedFrameTime > _maximumElapsedTime) _accumulatedFrameTime = _maximumElapsedTime;

		Frame();
	}
}

void Program::Frame()
{
	if (_isFixedUpdate)
	{
		_frameTime = _targetElapsedTime;

		uint8_t stepCount = 0;
		while (!_isExiting && _accumulatedFrameTime >= _targetElapsedTime)
		{
			_accumulatedFrameTime -= _targetElapsedTime;
			stepCount++;
			Update();
		}

		if (stepCount == 1)
		{
			if (_frameLag > 0) _frameLag--;
		} else
		{
			_frameLag += stepCount - 1;
		}

		if (_isRunningSlowly)
		{
			if (_frameLag == 0) _isRunningSlowly = false;
		} else if (_frameLag >= 0)
		{
			_isRunningSlowly = true;
		}
	} else
	{
		_frameTime = _accumulatedFrameTime;
		_accumulatedFrameTime = millisecondsf(0.0f);
		Update();
	}

	if (!_isSender)
	{
		Draw();
	}
	_framesCounter++;
}

void Program::HandleEvents()
{
	char x[50];
	sprintf(x, "FPS: %d, Frame Lag: %d", _framesPerSecond, _frameLag);
	SDL_SetWindowTitle(_window, x);

	SDL_Event e;
	while (SDL_PollEvent(&e) != 0)
	{
		if (e.type == SDL_QUIT)
		{
			_isExiting = true;
		}
	}
}

void Program::Update()
{
	if (!_isSender)
	{
		_framesPerSecondTimer += _frameTime;
		if (_framesPerSecondTimer > _oneSecondDuration)
		{
			_framesPerSecondTimer -= _oneSecondDuration;
			_framesPerSecond = _framesCounter;
			_framesCounter = 0;

			char x[10];
			sprintf(x, "FPS: %d", _framesPerSecond);
			SDL_SetWindowTitle(_window, x);
		}

		HandleEvents();
	}

	if (_isRunningSlowly)
	{
		return;
	}

	if (_isSender)
	{
		auto image = _screenCapture.GetScreenFrameBuffer();
		_transcoder->FeedFrame(image);
		try
		{
			auto packet = _transcoder->NextPacket();
			SendFrameBuffer(packet->GetRawData(), packet->GetRawDataLen());
		}
		catch (Codec::EncoderException ee)
		{

		}

	} else
	{
		UpdateTextures();
	}
}

void Program::SocketReceive(Program* program)
{
	auto socketHandle = program->_socketHandle;
	auto receiveBufferSize = program->_socketReceiveBufferSize;
	uint8_t receiveBuffer[receiveBufferSize];
	ssize_t status = 0;
	auto expectedDataPackets = 0;
	auto retrievedDataPackets = 0;
	size_t frameBufferSize = 0;
	uint8_t* frameBuffer = nullptr;
	uint8_t* frameBufferDataPointer = frameBuffer;

	while (!program->_isExiting)
	{
		memset(receiveBuffer, 0, receiveBufferSize);
		status = recvfrom(socketHandle, receiveBuffer, receiveBufferSize, 0, nullptr, nullptr);
		LogVerboseOrCritical(LogCategory::NETWORK, status != -1, "recvfrom");

		auto bytesRead = status;
		if (bytesRead == 8)
		{
			memcpy(&expectedDataPackets, receiveBuffer, sizeof(uint32_t));
			retrievedDataPackets = 0;
			size_t newFrameBufferSize = 0;
			memcpy(&newFrameBufferSize, receiveBuffer + 4, sizeof(uint32_t));
			if (newFrameBufferSize > frameBufferSize)
			{
				frameBufferSize = newFrameBufferSize;
				frameBuffer = (uint8_t*)realloc(frameBuffer, frameBufferSize);
				LogVerboseOrCritical(LogCategory::NETWORK, frameBuffer != nullptr, "realloc");
			}
			frameBufferDataPointer = frameBuffer;
			LogVerbose(LogCategory::NETWORK, "Received header packet for encoded frame buffer: size %d bytes, expecting %d data packet(s).", frameBufferSize, expectedDataPackets);
		} else
		{
			if (expectedDataPackets == 0 || expectedDataPackets == retrievedDataPackets)
			{
				LogVerbose(LogCategory::NETWORK, "Unexpected packet for encoded frame buffer.");
				continue;
			}

			memcpy(frameBufferDataPointer, receiveBuffer, (size_t)bytesRead);
			frameBufferDataPointer += bytesRead;
			retrievedDataPackets++;

			LogVerbose(LogCategory::NETWORK, "Received %d out of %d data packets for encoded frame buffer.",
			           retrievedDataPackets, expectedDataPackets);

			if (retrievedDataPackets == expectedDataPackets)
			{
				expectedDataPackets = 0;
				auto transcoder = program->_transcoder;
        Codec::VPXPacket nxtFramePkt(frameBuffer, (uint64_t)frameBufferDataPointer - (uint64_t)frameBuffer);
        try{
          program->_transcoder->FeedPacket(&nxtFramePkt);
        }
        catch(Codec::DecoderException de){
          //for whatever reason decoding failed
        }
				//TODO: Send packet to transcoder.
//				auto transcoderPacket = Codec::Packet()
//				transcoder->FeedPacket(transcoderPacket);
			}
		}


	}
}

void Program::SendFrameBuffer(const uint8_t* data, uint32_t dataLength)
{
	uint8_t header[8];
	memset(header, 0, sizeof(uint8_t) * 5);
	auto packetsCount = (uint32_t)ceil((float)dataLength / _socketSendBufferSize);
	memcpy(header, &packetsCount, sizeof(uint32_t));
	memcpy(header + 4, &dataLength, sizeof(uint32_t));
	int status = 0;
	status = sendto(_socketHandle, header, sizeof(uint8_t) * 8, 0, (struct sockaddr*)&_socketSendAddress,
	                     sizeof(_socketSendAddress));
	LogVerboseOrCritical(LogCategory::NETWORK, status != -1, "send");

	auto bytesRemaining = dataLength;
	auto dataPointer = data;
	for (int i = 0; i < packetsCount; i++)
	{
		auto length = std::min(_socketSendBufferSize, bytesRemaining);
		status = sendto(_socketHandle, dataPointer, length, 0, (struct sockaddr*)&_socketSendAddress,
		                sizeof(_socketSendAddress));
		LogVerbose(LogCategory::NETWORK, "ERRNO: %d \n", errno);
		LogVerboseOrCritical(LogCategory::NETWORK, status != -1, "send");
		bytesRemaining -= length;
		dataPointer += length;
	}

	LogVerbose(LogCategory::NETWORK, "Encoded frame buffer of size %d bytes sent as %d packets. (%d bytes per packet)", dataLength, packetsCount, _socketSendBufferSize);
}

void Program::UpdateTextures()
{
	try
	{
		auto image = _transcoder->NextImage();
		_texture->UploadData(&image->GetRGBBuffer()->at(0));
	}
	catch (Codec::DecoderException de)
	{

	}
}

void Program::Draw()
{
	glClearColor(0, 0, 0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	_texture->BindTexture();

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, nullptr);

	SDL_GL_SwapWindow(_window);
}
