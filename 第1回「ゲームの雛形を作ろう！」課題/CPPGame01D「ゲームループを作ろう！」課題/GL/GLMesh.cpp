//******************************************************************************
//
//
//		メッシュ
//
//
//******************************************************************************

#include "GLAll.h"

#define GLMESH_MAX			100			// メッシュ数（デフォルト値）

//==============================================================================
//
//		リソース
//
//==============================================================================

// コンストラクタ
GLMesh::GLMesh() : pMesh(0)
{
}

//==============================================================================

// デストラクタ
GLMesh::~GLMesh()
{
	release();
}

//==============================================================================

// メッシュのロード
void GLMesh::load(LPDIRECT3DDEVICE9 pd3dDevice, char *filename)
{
	release();

	// メッシュのロード
	LPD3DXBUFFER	pD3DXMtrlBuffer;
	if (FAILED(D3DXLoadMeshFromXA(filename, D3DXMESH_SYSTEMMEM, pd3dDevice, NULL, &pD3DXMtrlBuffer, NULL, &dwNumMaterials, &pMesh))) {
		release();
		return;
	}

	// Xファイルのパス取得
	char	strPath[MAX_PATH];
	ZeroMemory(strPath, sizeof(strPath));
	strncpy_s(strPath, MAX_PATH, filename, _TRUNCATE);
	for (int i = MAX_PATH - 1; i >= 0; i--) {
		if (strPath[i] == '/') break;
		strPath[i] = '\0';
	}

	// マテリアルとテクスチャの設定
	D3DXMATERIAL*	d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();
	pMaterials = new D3DMATERIAL9[dwNumMaterials];
	pTextures = new LPDIRECT3DTEXTURE9[dwNumMaterials];

	for (DWORD i = 0; i < dwNumMaterials; i++) {
		// マテリアルの設定
		pMaterials[i] = d3dxMaterials[i].MatD3D;				// マテリアルのコピー
		pMaterials[i].Ambient = pMaterials[i].Diffuse;			// アンビエントに頂点色をコピー

		// テクスチャの設定
		pTextures[i] = nullptr;
		if ((d3dxMaterials[i].pTextureFilename) && (strlen(d3dxMaterials[i].pTextureFilename) > 0)) {
			char	strTexture[MAX_PATH];
			strncpy_s(strTexture, MAX_PATH, strPath, _TRUNCATE);
			strcat_s(strTexture, MAX_PATH, d3dxMaterials[i].pTextureFilename);
			//pTextures[i] = D3D_LoadTexture(strTexture, colorKey);
			HRESULT hr = D3DXCreateTextureFromFileA(pd3dDevice, strTexture, &pTextures[i]);
			if (FAILED(hr)) {
				//MessageBox( "テクスチャが見つかりません！", strTexture );
				pTextures[i] = nullptr;
			}
		}
	}

	// バッファの解放
	pD3DXMtrlBuffer->Release();
}

//==============================================================================

// メッシュの解放
void GLMesh::release()
{
	if (!pMesh) return;

	// マテリアル配列の解放
	if (pMaterials) delete[] pMaterials;

	// 個々のテクスチャとテクスチャ配列の解放
	if (pTextures) {
		for (DWORD i = 0; i < dwNumMaterials; i++) {
			RELEASE(pTextures[i]);
		}
		delete[] pTextures;
	}

	// メッシュ（ジオメトリ）の解放
	if (pMesh) pMesh->Release();
	pMesh = nullptr;
}

//==============================================================================
//
//		リソース管理
//
//==============================================================================

// 初期設定
void GLMeshManager::Init()
{
	Resize(GLMESH_MAX);
}
// 終了処理
void GLMeshManager::Uninit()
{
	m_resource.clear();
}

//==============================================================================

// 数の変更
void GLMeshManager::Resize(int n)
{
	Uninit();				// 念のため
	m_resource.resize(n);
}
// ロード
void GLMeshManager::Load(LPDIRECT3DDEVICE9 pd3dDevice, LOAD_MESH* pLoad)
{
	while ((UINT)pLoad->meshNum < m_resource.size()) {
		m_resource[pLoad->meshNum].load(pd3dDevice, pLoad->filename);
		pLoad++;
	}
}
// メッシュ解放（すべて）
void GLMeshManager::ReleaseAll()
{
	for (auto& res : m_resource) res.release();
}

//==============================================================================

// リソースを返す
GLMesh* GLMeshManager::getResource(int i)
{
	return ((UINT)i < m_resource.size()) ? &m_resource[i] : nullptr;
}

//==============================================================================
//
//		描画コマンド
//
//==============================================================================

// 範囲チェック
static float rangeCheck(float color)
{
	if (color < 0.0f) color = 0.0f;
	if (color > 1.0f) color = 1.0f;
	return color;
}

//==============================================================================

// メッシュ描画
GLDrawMesh::GLDrawMesh(MESH_DATA* pMesh, D3DXMATRIX& matTrans, float alpha, float r, float g, float b)
{
	m_matTrans = pMesh->matData * matTrans;
	m_pMesh = pGLMeshManager->getResource(pMesh->meshNum);
	m_alpha = alpha;
	m_r = r;
	m_g = g;
	m_b = b;
}

void GLDrawMesh::execute(LPDIRECT3DDEVICE9 pd3dDevice)
{
	if (!m_pMesh) return;

	pd3dDevice->SetTransform(D3DTS_WORLD, &m_matTrans);

	// 描画
	for (DWORD i = 0; i < m_pMesh->dwNumMaterials; i++) {
		D3DMATERIAL9	mtrl = m_pMesh->pMaterials[i];
		mtrl.Diffuse.a *= m_alpha;
		mtrl.Diffuse.r = rangeCheck(mtrl.Diffuse.r + m_r);
		mtrl.Diffuse.g = rangeCheck(mtrl.Diffuse.g + m_g);
		mtrl.Diffuse.b = rangeCheck(mtrl.Diffuse.b + m_b);
		pd3dDevice->SetMaterial(&mtrl);							// マテリアル設定
		pd3dDevice->SetTexture(0, m_pMesh->pTextures[i]);		// テクスチャ設定
		m_pMesh->pMesh->DrawSubset(i);							// メッシュ描画
	}
}

//******************************************************************************
