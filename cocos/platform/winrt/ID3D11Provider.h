#ifndef _IWINRTWINDOW_H__
#define _IWINRTWINDOW_H__

#include <d3d11_1.h>
#include <cocos2d.h>

NS_CC_BEGIN

class ID3D11Provider {
public:
	virtual ID3D11Device2*                 GetDevice() = 0;
	virtual ID3D11DeviceContext2*          GetContext() = 0;
	virtual ID3D11DepthStencilView*        GetDepthStencilView() = 0;
	virtual ID3D11RenderTargetView* const* GetRenderTargetView() const = 0;
};

NS_CC_END

#endif // _IWINRTWINDOW_H__