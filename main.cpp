#include "MyWhatTheGolf.h"
#include "AssetLoader.h"
#include "Object.h"

// TODO :: 1. SimpleMath -> XM

/*
 * TODO QUEUE
 * 1. move vertexConstantBuffer to dxData
 * 2. ������ ���� windowManager�� ������ �ֱ�, ���ÿ� window manager initialize �Լ� static ����
 */

// struct MeshData
//{
//	std::vector<Vertex>	  vertices;
//	std::vector<uint32_t> indices;
//	std::string			  textureFilename;
// };

// struct PixelConstantData
//{
//	DirectX::SimpleMath::Vector4 color[3];
//	DirectX::SimpleMath::Vector4 padding;
// };
// static_assert((sizeof(PixelConstantData) % 16 == 0), "Pixel Constant Buffer size must be 16-byte aligned.");

// ����� ���ؼ� main �Լ� ���. �� �Լ� ����Ϸ��� sln -> properties -> Linker -> System -> Subsystem -> Windows�� ����
// int main()
// int WINAPI wWinMain(HINSTANCE hinstance, HINSTANCE, char*, int)

// int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int)
int main()
{
	const int	  width = 1280;
	const int	  height = 960;
	MyWhatTheGolf app(width, height);

	if (!app.Initialize())
		return 1;

	app.Run();

	return 0;
}
