#pragma once

//=============================================
// ## 18.12.03 ## image Class ## 취업 100% 합시다
//=============================================

#include "animation.h"

class image
{
public:

	enum IMAGE_LOAD_KIND
	{
		LOAD_RESOURCE,			//리소스로부터 이미지를
		LOAD_FILE,				//파일로부터 이미지를
		LOAD_EMPTY,				//빈 BMP파일
		LOAD_END
	};

	typedef struct tagImageInfo
	{

		DWORD		resID;
		HDC			hMemDC;
		HBITMAP		hBit;
		HBITMAP		hOBit;
		float		x;				//이미지의 X좌표에 사용될 변수
		float		y;				//이미지의 Y좌표에 사용될 변수
		int			width;			//이미지의 전체 가로크기
		int			height;			//이미지의 전체 세로크기
		int			currentFrameX;	//현재 프레임 가로번호 (X열)
		int			currentFrameY;	//현재 프레임 세로번호 (Y열)
		int			maxFrameX;		//최대 프레임 가로갯수 (X열)
		int			maxFrameY;		//최대 프레임 세로갯수 (Y열)
		int			frameWidth;		//프레임이미지의 가로크기
		int			frameHeight;	//프레임이미지의 세로크기
		BYTE		loadType;

		tagImageInfo()
		{
			resID = NULL;
			hMemDC = NULL;
			hBit = NULL;
			hOBit = NULL;
			x = NULL;
			y = NULL;
			width = NULL;
			height = NULL;
			currentFrameX = NULL;
			currentFrameY = NULL;
			maxFrameX = NULL;
			maxFrameY = NULL;
			frameWidth = NULL;
			frameHeight = NULL;
			loadType = LOAD_RESOURCE;
		}
	}IMAGE_INFO, *LPIMAGE_INFO;

private:
	LPIMAGE_INFO		_imageInfo;		//이미지 정보를 담은 구조체
	CHAR*				_fileName;		//파일이름에 사용할 캐릭터포인터
	BOOL				_trans;			//특정 칼라를 뺄꺼냐?
	COLORREF			_transColor;	//제외할 칼라의 RGB 값은?
	BOOL				_alpha;			//알파할거임?
	BLENDFUNCTION		_blendFunc;		//알파블렌드 관련 함수를 포함
	LPIMAGE_INFO		_blendImage;	//알파처리할 이미지

public:
	image();
	~image();

	//빈 비트맵 이미지 생성
	HRESULT init(int width, int height, BOOL alpha = FALSE);

	//리소스로부터 읽어들일 경우
	HRESULT init(const DWORD resID, int width, int height,
		BOOL trans = FALSE, COLORREF transColor = FALSE, BOOL alpha = FALSE);

	//이미지 초기화(파일이름, 이미지가로크기, 이미지세로크기, 특정칼라제외여부, 특정칼라값)
	HRESULT init(const char* fileName, int width, int height,
		BOOL trans = FALSE, COLORREF transColor = RGB(0, 0, 0), BOOL alpha = FALSE);

	//파일로부터 이미지를 읽어들이는 함수 (파일제목, 이미지 뿌릴좌표 X, Y 이미지 가로세로크기)
	HRESULT init(const char* fileName, float x, float y, int width, int height,
		BOOL trans = FALSE, COLORREF transColor = FALSE, BOOL alpha = FALSE);

	//파일로부터 이미지 읽어들이는 함수 (프레임이미지 용)
	HRESULT init(const char* fileName, float x, float y, int width, int height,
		int frameX, int frameY, BOOL trans = FALSE, COLORREF transColor = FALSE, BOOL alpha = FALSE);

	//파일로부터 이미지 읽어들이는 함수 (프레임이미지 용)
	HRESULT init(const char* fileName, int width, int height, int frameX, int frameY,
		BOOL trans = FALSE, COLORREF transColor = FALSE, BOOL alpha = FALSE);

	void release();
	void alphaRelease();


	void setTransColor(BOOL trans, COLORREF transColor);
	HRESULT setAlpha(BOOL alpha);


	void render(HDC hdc);
	//이미지 렌더함수(뿌려줄DC, 
	void render(HDC hdc, int destX, int destY);

	//이미지 렌더함수(뿌려줄DC, 
	void render2(HDC hdc, int destX, int destY);

	//이미지 렌더함수(뿌려줄DC 뿌려줄 left, top, 이미지 가져올 Left, Top, Width, Height)
	void render(HDC hdc, int destX, int destY,
		int sourX, int sourY, int sourWidth, int sourHeight);

	//이미지 프레임 렌더(뿌려줄 DC, 뿌려줄 X(Left), Y(Top))
	void frameRender(HDC hdc, int destX, int destY);
	//이미지 프레임 렌더(뿌려줄 DC, 뿌려줄 Left, 뿌려줄 Top, 프레임X번호, 프레임Y번호)
	void frameRender(HDC hdc, int destX, int destY, int currentFrameX, int currentFrameY);

	//이미지 한 장으로 루프렌더링하기(뿌려줄DC, 뿌려줄영역(렉트), 루프방향X, 루프방향Y)
	void loopRender(HDC hdc, const LPRECT drawArea, int offSetX, int offSetY);

	void alphaRender(HDC hdc, BYTE alpha);

	void alphaRender(HDC hdc, int destX, int destY, BYTE alpha);

	void alphaRender(HDC hdc, int destX, int destY,
		int sourX, int sourY, int sourWidth, int sourHeight, BYTE alpha);

	//이미지 프레임 렌더(뿌려줄 DC, 뿌려줄 X(Left), Y(Top))
	void alphaFrameRender(HDC hdc, int destX, int destY, BYTE alpha);
	//이미지 프레임 렌더(뿌려줄 DC, 뿌려줄 Left, 뿌려줄 Top, 프레임X번호, 프레임Y번호)
	void alphaFrameRender(HDC hdc, int destX, int destY, int currentFrameX, int currentFrameY, BYTE alpha);
	//이미지 한 장으로 루프렌더링하기(뿌려줄DC, 뿌려줄영역(렉트), 루프방향X, 루프방향Y)
	void alphaLoopRender(HDC hdc, const LPRECT drawArea, int offSetX, int offSetY, BYTE alpha);

	void aniRender(HDC hdc, int destX, int destY, animation* ani);

	inline HDC getMemDC() { return _imageInfo->hMemDC; }

	//=================================================
	// 이미지 관련 접근자, 설정자 이제부터 편하게 쓰세용 ^_^)/
	//=================================================

	//이미지 좌표 X 
	inline void SetX(float x) { _imageInfo->x = x; }
	inline float GetX() { return _imageInfo->x; }

	//이미지 좌표 Y
	inline void SetY(float y) { _imageInfo->y = y; }
	inline float GetY() { return _imageInfo->y; }

	//이미지 중점좌표(X, Y)
	inline void SetCenter(float x, float y)
	{
		_imageInfo->x = x - (_imageInfo->width / 2);
		_imageInfo->y = y - (_imageInfo->height / 2);
	}

	inline float GetCenterX()
	{
		return _imageInfo->maxFrameX <= 0 ?
			_imageInfo->x + (_imageInfo->width / 2) :
			_imageInfo->x + (_imageInfo->frameWidth / 2);
	}

	inline float GetCenterY()
	{
		return _imageInfo->maxFrameY <= 0 ?
			_imageInfo->y + (_imageInfo->height / 2) :
			_imageInfo->y + (_imageInfo->frameHeight / 2);
	}

	//가로 크기
	inline int GetWidth() { return _imageInfo->width; }

	//세로 크기
	inline int GetHeight() { return _imageInfo->height; }

	inline void SetFrameX(int frameX)
	{
		_imageInfo->currentFrameX = frameX;

		//예외처리 현재 프레임번호가 최대프레임번호보다 크면 최대프레임번호로 맞춰라
		if (frameX > _imageInfo->maxFrameX)
			_imageInfo->currentFrameX = _imageInfo->maxFrameX;
	}

	inline void SetFrameY(int frameY)
	{
		_imageInfo->currentFrameY = frameY;

		if (frameY > _imageInfo->maxFrameY)
			_imageInfo->currentFrameY = _imageInfo->maxFrameY;
	}

	inline int getMaxFrameX() { return _imageInfo->maxFrameX; }
	inline int getMaxFrameY() { return _imageInfo->maxFrameY; }

	inline int getFrameX() { return _imageInfo->currentFrameX; }
	inline int getFrameY() { return _imageInfo->currentFrameY; }

	inline int getFrameWidth() { return _imageInfo->frameWidth; }
	inline int getFrameHeight() { return _imageInfo->frameHeight; }


};

