//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: xfile.cpp
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Demonstrates how to load and render an XFile.
//          
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "d3dUtility.h"
#include "d3dx9tex.h"
#include <vector>
#include <string.h>
#include <string>
#include <iostream>

using namespace std;

#define NUM_OF_MESH 14
#define CAMERA_NUM 14

//
// Globals
//

IDirect3DDevice9* Device = 0;

const int Width = 1366;
const int Height = 768;

ID3DXMesh* Mesh[NUM_OF_MESH] = { 0 };
D3DXMATRIX PersonMatrices[NUM_OF_MESH];
D3DXMATRIX PersonTransition;
D3DXMATRIX PersonRotation;
D3DXMATRIX FixedTransition[NUM_OF_MESH];
D3DXMATRIX FixedRotation[NUM_OF_MESH];
D3DXMATRIX WalkRotation[NUM_OF_MESH];

float CameraView = 1.0f;
float theta = 0.0f;


//ID3DXMesh* Mesh = 0;

vector<vector< D3DMATERIAL9>>        Mtrls(NUM_OF_MESH, vector<D3DMATERIAL9>(0));
vector<vector<IDirect3DTexture9*>> Textures(NUM_OF_MESH, vector<IDirect3DTexture9*>(0));


struct BoneSkeleton
{
	void set(D3DXVECTOR3 p, float xRot, float yRot, float zRot, int parentIndex, float s)
	{
		pos = p;
		xAngle = xRot;
		yAngle = yRot;
		zAngle = zRot;
		parent = parentIndex;
		size = s;
	}


	D3DXVECTOR3 pos;  // Relative to parent frame.
	float xAngle;	  // XRelative to parent frame.
	float yAngle;	  // YRelative to parent frame.
	float zAngle;	  // ZRelative to parent frame.
	int parent;       // Index to parent frame (-1 if root, i.e., no parent)
	float size;       // Relative to world frame.
	D3DXMATRIX toParentXForm;
	D3DXMATRIX toWorldXForm;
};


BoneSkeleton Body[NUM_OF_MESH + 1];
//Positions
D3DXVECTOR3 pos[NUM_OF_MESH + 1] =
{
	D3DXVECTOR3(0.0f, -2.0f, 0.0f),     // Pan
	D3DXVECTOR3(0.0f, 1.0f, 0.0f),     // Body
	D3DXVECTOR3(0.0f, 1.25f, 0.0f),	   // Head
	D3DXVECTOR3(-0.75f, 0.250f, 0.0f),	   // RBAZOO
	D3DXVECTOR3(-0.75f, -2.0f, -0.5f),   // RHAND
	D3DXVECTOR3(1.0f, 0.250f, 0.0f),	   // LBAZOO
	D3DXVECTOR3(0.25f, -2.0f, -0.5f),    // LHAND
	D3DXVECTOR3(-0.5f, -1.2f, 0.0f),	   // RLEG
	D3DXVECTOR3(-0.25f, -2.5f, 0.0f),	   // RFOOT
	D3DXVECTOR3(0.5f, -1.2f, 0.0f),	   // LLEG
	D3DXVECTOR3(1.0f, -2.5f, 0.0f),	   // LFOOT
	D3DXVECTOR3(0.0f, 7.0f, 15.0f),	   // CLUB
	D3DXVECTOR3(0.0f, -6.0f, -4.0f),	   // BARBEL
	D3DXVECTOR3(-13.0f, -6.0f, 25.0f),	   // BARBEL2
	D3DXVECTOR3(0.0f, -1.25f, 8.0f)	   // CAM

};



void buildObjectWorldTransforms();
void IDLE();

//
// Framework functions
//

bool Setup()
{
	HRESULT hr[NUM_OF_MESH] = { 0 };
	//
	// Load the XFile data.
	//

	//ID3DXBuffer* adjBuffer[NUM_OF_MESH] = { 0 };
	//ID3DXBuffer* mtrlBuffer[NUM_OF_MESH] = { 0 };
	//DWORD        numMtrls[NUM_OF_MESH]   = {0};


	ID3DXBuffer* adjBuffer[NUM_OF_MESH] = { 0 };
	ID3DXBuffer* mtrlBuffer[NUM_OF_MESH] = { 0 };
	DWORD        numMtrls[NUM_OF_MESH] = { 0 };

	string MeshNames[NUM_OF_MESH] = { "HEAP.x" ,"BODY.x", "HEAD2.x",  "RBAZ.x", "RH.x", "LBAZ.x", "LH.x", "RL.x", "RF.x", "LL.x", "LF.x",
		"FinalRoom3.x", "Barbel.x", "Barbel.x" };





	Body[0].set(pos[0], 0.0f, 0.0f, 0.0f, -1, 5.0f);  // Pan
	Body[1].set(pos[1], 0.0f, 0.0f, 0.0f, 0, 5.0f);   // Body
	Body[2].set(pos[2], 0.0f, 0.0f, 0.0f, 1, 0.05f);    // Head
	Body[3].set(pos[3], 0.0f, 0.0f, 0.0f, 1, 5.0f);    // RBAZOO
	Body[4].set(pos[4], 0.0f, 0.0f, 0.0f, 3, 5.0f);    // RHAND
	Body[5].set(pos[5], 0.0f, 0.0f, 0.0f, 1, 5.0f);    // LBAZOO
	Body[6].set(pos[6], 0.0f, 0.0f, 0.0f, 5, 5.0f);    // LHAND
	Body[7].set(pos[7], 0.0f, 0.0f, 0.0f, 0, 5.0f);    // RLEG
	Body[8].set(pos[8], 0.0f, 0.0f, 0.0f, 7, 5.0f);    // RFOOT
	Body[9].set(pos[9], 0.0f, 0.0f, 0.0f, 0, 5.0f);    // LLEG
	Body[10].set(pos[10], 0.0f, 0.0f, 0.0f, 9, 5.0f);   // LFOOT
	Body[11].set(pos[11], 0.0f, 0.0f, 0.0f, -1, 3.0f);   //CLUB
	Body[12].set(pos[12], 0.0f, 0.0f, 0.0f, -1, 0.05f);   //BARBEL
	Body[13].set(pos[13], 0.0f, 0.0f, 0.0f, -1, 0.08f);   //BARBEL2
	Body[CAMERA_NUM].set(pos[CAMERA_NUM], 0.0f, 0.0f, 0.0f, 2, 0.05f);  //CAM


	IDirect3DTexture9* tex = 0;
	IDirect3DTexture9* pers = 0;
	D3DXCreateTextureFromFile(
		Device,
		"armor.jpg",
		&pers);
	for (int j = 0; j < NUM_OF_MESH; j++)
	{
		hr[j] = D3DXLoadMeshFromX(
			MeshNames[j].c_str(),
			D3DXMESH_MANAGED,
			Device,
			&adjBuffer[j],
			&mtrlBuffer[j],
			0,
			&numMtrls[j],
			&Mesh[j]);

		if (FAILED(hr[j]))
		{
			::MessageBox(0, "D3DXLoadMeshFromX() - FAILED", 0, 0);
			return false;
		}




		//
		// Extract the materials, and load textures.
		//

		if (mtrlBuffer[j] != 0 && numMtrls[j] != 0)
		{
			D3DXMATERIAL* mtrls = (D3DXMATERIAL*)mtrlBuffer[j]->GetBufferPointer();

			for (int i = 0; i < numMtrls[j]; i++)
			{
				// the MatD3D property doesn't have an ambient value set
				// when its loaded, so set it now:
				mtrls[i].MatD3D.Ambient = mtrls[i].MatD3D.Diffuse;

				// save the ith material
				Mtrls[j].push_back(mtrls[i].MatD3D);

				// check if the ith material has an associative texture
				if (mtrls[i].pTextureFilename != 0)
				{
					// yes, load the texture for the ith subset
					if (j > 10) {
						D3DXCreateTextureFromFile(
							Device,
							mtrls[i].pTextureFilename,
							&tex);
						// save the loaded texture
						Textures[j].push_back(tex);
					}
					else {
						Textures[j].push_back(pers);
					}
				}
				else
				{
					// no texture for the ith subset
					Textures[j].push_back(0);
				}
			}
		}
		d3d::Release<ID3DXBuffer*>(mtrlBuffer[j]); // done w/ buffer


	//
	// Optimize the mesh.
	////
	//for (int i = 2; i < NUM_OF_MESH; i++) {
	//	hr[i] = Mesh[i]->OptimizeInplace(
	//		D3DXMESHOPT_ATTRSORT |
	//		D3DXMESHOPT_COMPACT |
	//		D3DXMESHOPT_VERTEXCACHE,
	//		(DWORD*)adjBuffer->GetBufferPointer(),
	//		0, 0, 0);
	//}


		d3d::Release<ID3DXBuffer*>(adjBuffer[j]); // done w/ buffer

	}
	if (FAILED(hr))
	{
		::MessageBox(0, "OptimizeInplace() - FAILED", 0, 0);
		return false;
	}

	//
	// Set texture filters.
	//

	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	// 
	// Set Lights.
	//

	D3DXVECTOR3 dir(1.0f, -1.0f, -1.0f);
	D3DXCOLOR col(1.0f, 1.0f, 1.0f, 1.0f);
	D3DLIGHT9 light = d3d::InitDirectionalLight(&dir, &col);

	Device->SetLight(0, &light);
	Device->LightEnable(0, true);
	Device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	Device->SetRenderState(D3DRS_SPECULARENABLE, true);

	//
	// Set camera.
	//


	//
	// Set projection matrix.
	//

	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
		&proj,
		D3DX_PI * 0.5f, // 90 - degree
		(float)Width / (float)Height,
		1.0f,
		10000.0f);
	Device->SetTransform(D3DTS_PROJECTION, &proj);


	return true;
}

void Cleanup()
{
	for (int j = 0; j < NUM_OF_MESH; j++) {

		d3d::Release<ID3DXMesh*>(Mesh[j]);
		for (int i = 0; i < Textures[j].size(); i++)
			d3d::Release<IDirect3DTexture9*>(Textures[j][i]);
	}
}

void SetCamera() {
	D3DXMATRIX h, p;
	p = Body[CAMERA_NUM].toWorldXForm;
	h = Body[0].toWorldXForm;
	D3DXVECTOR3 position(p._41, p._42 + 2.25f, p._43);
	D3DXVECTOR3 target(h._41, h._42 + 2.25f, h._43);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);

	D3DXMATRIX V;
	D3DXMatrixLookAtLH(
		&V,
		&position,
		&target,
		&up);

	Device->SetTransform(D3DTS_VIEW, &V);
}


void ChangePosition(float speed) {
	speed *= -1;
	Body[0].pos.z += speed * cosf(theta);
	Body[0].pos.x += speed * sinf(theta);


}

void Walk(bool up, float dt) {
	static bool foot = true; //right 1, left 0
	static int orientation = 1; //straight 1, back -1
	static int lastMove = 0; //straight 1, back -1 , null 0
	if (up) {
		if (lastMove == -1 || lastMove == 0) {
			orientation = 1;
			Body[7].xAngle = 0.0f;
			Body[8].xAngle = 0.0f;
			Body[9].xAngle = 0.0f;
			Body[10].xAngle = 0.0f;


		}
		lastMove = 1;
		if (foot) {
			Body[5].xAngle += orientation * 2.0f * dt;
			Body[6].xAngle += orientation * 2.0f * dt;
			Body[7].xAngle += orientation * 2.0f * dt;
			Body[8].xAngle -= orientation * 4.0f * dt;
			if (Body[7].xAngle > 0.4f || Body[6].xAngle > 0.4f || Body[5].xAngle > 0.4f) {
				orientation *= -1;
				Body[5].xAngle = 0.35f;
				Body[6].xAngle = 0.35f;
				Body[7].xAngle = 0.35f;
			}
			if (Body[7].xAngle < 0.0f || Body[6].xAngle < 0.0f || Body[5].xAngle < 0.0f) {
				foot = false;
				orientation *= -1;
				Body[5].xAngle = 0.0f;
				Body[6].xAngle = 0.0f;
				Body[7].xAngle = 0.0f;
				Body[8].xAngle = 0.0f;
			}
		}
		else
		{
			Body[3].xAngle += orientation * 2.0f * dt;
			Body[4].xAngle += orientation * 2.0f * dt;
			Body[9].xAngle += orientation * 2.0f * dt;
			Body[10].xAngle -= orientation * 4.0f * dt;
			if (Body[9].xAngle > 0.4f || Body[4].xAngle > 0.4f || Body[3].xAngle>0.4f) {
				orientation *= -1;
				Body[3].xAngle = 0.35f;
				Body[4].xAngle = 0.35f;
				Body[9].xAngle = 0.35f;
			}
			if (Body[9].xAngle < 0.0f || Body[4].xAngle < 0.0f || Body[3].xAngle < 0.0f) {
				foot = true;
				orientation *= -1;
				Body[3].xAngle = 0.0f;
				Body[4].xAngle = 0.0f;
				Body[9].xAngle = 0.0f;
				Body[10].xAngle = 0.0f;
			}
		}
		ChangePosition(0.08);
	}
	else {
		if (lastMove == 1 || lastMove == 0) {
			orientation = -1;
			Body[7].xAngle = 0.0f;
			Body[8].xAngle = 0.0f;
			Body[9].xAngle = 0.0f;
			Body[10].xAngle = 0.0f;

		}
		lastMove = -1;

		if (foot) {
			Body[5].xAngle += orientation * 1.0f * dt;
			Body[7].xAngle += orientation * 1.0f * dt;
			if (Body[7].xAngle < -0.4f || Body[5].xAngle < -0.4f) {
				orientation *= -1;
				Body[5].xAngle = -0.35f;
				Body[7].xAngle = -0.35f;
			}
			if (Body[7].xAngle > 0.0f || Body[5].xAngle > 0.0f) {
				foot = false;
				orientation *= -1;
				Body[5].xAngle = 0.0f;
				Body[7].xAngle = 0.0f;
			}
		}
		else
		{
			Body[3].xAngle += orientation * 1.0f * dt;
			Body[9].xAngle += orientation * 1.0f * dt;
			if (Body[9].xAngle < -0.4f || Body[3].xAngle < -0.4f) {
				orientation *= -1;
				Body[3].xAngle = -0.35f;
				Body[9].xAngle = -0.35f;
			}
			if (Body[9].xAngle > 0.0f || Body[3].xAngle > 0.0f) {
				foot = true;
				orientation *= -1;
				Body[3].xAngle = 0.0f;
				Body[9].xAngle = 0.0f;
			}
		}
		ChangePosition(-0.05);
	}
}

void Run(float dt) {
	static bool foot = true; //right 1, left 0
	static int orientation = 1; //straight 1, back -1
	Body[1].xAngle = -0.2f;
	if (foot) {
		Body[5].xAngle += orientation * 7.0f * dt;
		Body[6].xAngle += orientation * 7.0f * dt;
		Body[7].xAngle += orientation * 7.0f * dt;
		Body[8].xAngle -= orientation * 7.0f * dt;
		if (Body[7].xAngle >= 1.0f || Body[6].xAngle >= 1.0f || Body[5].xAngle >= 1.0f) {
			orientation *= -1;
			Body[5].xAngle = 0.95f;
			Body[6].xAngle = 0.95f;
			Body[7].xAngle = 0.95f;
		}
		if (Body[7].xAngle <= 0.0f || Body[6].xAngle <= 0.0f || Body[5].xAngle <= 0.0f) {
			foot = false;
			orientation *= -1;
			Body[5].xAngle = 0.0f;
			Body[6].xAngle = 0.0f;
			Body[7].xAngle = 0.0f;
			Body[8].xAngle = 0.0f;
		}
	}
	else
	{
		Body[3].xAngle += orientation * 7.0f * dt;
		Body[4].xAngle += orientation * 7.0f * dt;
		Body[9].xAngle += orientation * 7.0f * dt;
		Body[10].xAngle -= orientation * 7.0f * dt;
		if (Body[9].xAngle >= 1.0f || Body[3].xAngle >= 1.0f || Body[4].xAngle >= 1.0f) {
			orientation *= -1;
			Body[3].xAngle = 0.95f;
			Body[4].xAngle = 0.95f;
			Body[9].xAngle = 0.95f;
		}
		if (Body[9].xAngle <= 0.0f || Body[3].xAngle <= 0.0f || Body[4].xAngle <= 0.0f) {
			foot = true;
			orientation *= -1;
			Body[3].xAngle = 0.0f;
			Body[4].xAngle = 0.0f;
			Body[9].xAngle = 0.0f;
			Body[10].xAngle = 0.0f;
		}
	}
	ChangePosition(0.25);
}


void IDLE() {
	for (int i = 0; i < NUM_OF_MESH; i++) {
		Body[i].xAngle = 0.0f;
	}
	Body[3].pos = pos[3];
	Body[5].pos = pos[5];
}

void Rotate() {
	theta += Body[2].yAngle;
	Body[0].yAngle = theta;
	Body[2].yAngle = 0.0f;
	if (theta >= 2 * D3DX_PI)
		theta = 0.0f;
}

void BarbelLift(float dt, int &step, bool &animate, int type) {
	//STEP 1
	if (Body[3].xAngle <= 2.5f && step == 1) {
		Body[3].xAngle += 2.0f * dt;
		Body[5].xAngle += 2.0f * dt;
		Body[3].pos.y += dt / 2;
		Body[5].pos.y += dt / 2;
	}
	else if (step == 1) {
		step = 2;
	}
	//STEP 2
	if (step == 2 && Body[1].xAngle >= -2 * D3DX_PI / 3) {
		D3DXVECTOR3 diffrence;
		if(type==1)
			diffrence = Body[0].pos - Body[12].pos;
		else
			diffrence = Body[0].pos - Body[13].pos;
		if (sqrtf(D3DXVec3Dot(&diffrence, &diffrence)) < 8.0f) {
			Body[1].xAngle -= 2.0f * dt;
		}
		else {
			animate = false;
			step = 1;
		}
	}
	else if (step == 2)
	{

		step = 3;
		if (type == 1) {
			Body[12].parent = 6;
			Body[12].pos = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);
		}
		else
		{
			Body[13].parent = 6;
			Body[13].pos = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);
		}
	}
	//STEP 3
	if (step == 3 && Body[1].xAngle <= 0.0f) {
		Body[1].xAngle += 2.0f * dt;
		if (type == 2) {
			Body[3].xAngle -= 2.4f * dt;
			Body[5].xAngle -= 2.4f * dt;
			Body[3].pos.y -= dt / 2;
			Body[5].pos.y -= dt / 2;
		}
	}
	else if (step == 3) {
		if (type == 1)
			step = 4;
		else {
			step = 6;
			animate = false;
		}
		}

	//STEP 4
	if (step == 4 && Body[3].xAngle >= 0.0f) {
		Body[3].xAngle -= 2.0f * dt;
		Body[5].xAngle -= 2.0f * dt;
		Body[3].pos.y -= dt / 2;
		Body[5].pos.y -= dt / 2;
	}
	else if (step == 4)
		step = 5;
	//STEP 5
	if (Body[3].xAngle <= 2.5f && step == 5) {
		Body[3].xAngle += 2.0f * dt;
		Body[5].xAngle += 2.0f * dt;
		Body[3].pos.y += dt / 2;
		Body[5].pos.y += dt / 2;
	}
	else if (step == 5) {
		step = 4;
	}
	
	//STEP 6
	if (step == 6 && Body[3].xAngle <= 2.5f && type == 3) {
		Body[3].xAngle += 12.0f * dt;
		Body[5].xAngle += 12.0f * dt;
		Body[3].pos.y += 3.0f * dt;
		Body[5].pos.y += 3.0f * dt;
		Body[7].xAngle += 2.0f * dt;
		Body[9].xAngle -= 2.0f * dt;
	}
	else if (step == 6 && animate==true) {
		step = 7;
	}
	if (step == 7 && Body[7].xAngle > 0.0f) {
		Body[7].xAngle -= dt / 4;
		Body[9].xAngle += dt / 4;
		Body[0].pos.z -= 0.02f * cosf(theta);
		Body[0].pos.x -= 0.02f * sinf(theta);
	}
	
}

void BarbelFree() {
	Body[12].parent = -1;
	Body[12].pos.x = Body[12].toWorldXForm._41;
	Body[12].pos.y = pos[12].y;
	Body[12].pos.z = Body[12].toWorldXForm._43;

	Body[13].parent = -1;
	Body[13].pos.x = Body[13].toWorldXForm._41;
	Body[13].pos.y = pos[13].y;
	Body[13].pos.z = Body[13].toWorldXForm._43;
}

void HandleKeys(float dt) {
	static int count = 0;
	static bool animate = false;
	static int barberStep = 1;
	//UP
	if (::GetAsyncKeyState('W') & 0x8000f) {
		Rotate();
		count = 0;
		Walk(true, dt);
	}
	else
	{
		count++;
	}
	//RIGHT
	if (::GetAsyncKeyState('D') & 0x8000f) {
		if (Body[2].yAngle <= D3DX_PI / 2)
			Body[2].yAngle += 2.0f * dt;
	}
	//LEFT
	if (::GetAsyncKeyState('A') & 0x8000f) {
		if (Body[2].yAngle >= -D3DX_PI / 2)
			Body[2].yAngle -= 2.0f * dt;
	}
	//DOWN
	if (::GetAsyncKeyState('S') & 0x8000f) {
		Rotate();
		count = 0;
		Walk(false, dt);
	}
	else
	{
		count++;
	}
	if (::GetAsyncKeyState('R') & 0x8000f) {
		Rotate();
		count = 0;
		Run(dt);
	}
	else
	{
		count++;
	}
	//FRONT
	if (::GetAsyncKeyState('M') & 0x8000f) {
		Body[CAMERA_NUM].pos.z *= -1;
	}
	//LIFT
	if (::GetAsyncKeyState(' ') & 0x8000f) {
		if(barberStep!=6)
			animate = true;
		D3DXVECTOR3 diffrence( Body[0].toWorldXForm._41 - Body[12].toWorldXForm._41,0.0f, Body[0].toWorldXForm._41 - Body[12].toWorldXForm._41);
		D3DXVECTOR3 diffrence2(Body[0].toWorldXForm._41 - Body[13].toWorldXForm._41, 0.0f, Body[0].toWorldXForm._41 - Body[13].toWorldXForm._41);
		if (sqrtf(D3DXVec3Dot(&diffrence, &diffrence)) < sqrtf(D3DXVec3Dot(&diffrence2, &diffrence2))) {
			BarbelLift(dt, barberStep, animate, 1);
		}
		else {
			BarbelLift(dt, barberStep, animate, 2);
		}
	}
	if (::GetAsyncKeyState('T') & 0x8000f) {
		animate = true;
		BarbelLift(dt, barberStep, animate, 3);
	}
	//ON HAND
	if (::GetAsyncKeyState('V') & 0x8000f) {
		animate = false;
		if (Body[12].parent != -1)
			barberStep = 4;
		else if (Body[13].parent != -1)
			barberStep = 6;
		else
			barberStep = 1;
	}
	//FREE
	if (::GetAsyncKeyState('F') & 0x8000f) {
		BarbelFree();
		barberStep = 1;
	}

	if (count > 300 && !animate) {
		IDLE();
		if (count > 10000)
			count = 301;
	}


	//gDInput->poll();

	//// Check input.
	//if (gDInput->keyDown(DIK_W))
	//	mCameraHeight += 25.0f * dt;
	//if (gDInput->keyDown(DIK_S))
	//	mCameraHeight -= 25.0f * dt;

	//// Divide by 50 to make mouse less sensitive. 
	//mCameraRotationY += gDInput->mouseDX() / 100.0f;
	//mCameraRadius += gDInput->mouseDY() / 25.0f;

	//// If we rotate over 360 degrees, just roll back to 0
	//if (fabsf(mCameraRotationY) >= 2.0f * D3DX_PI)
	//	mCameraRotationY = 0.0f;

	//// Don't let radius get too small.
	//if (mCameraRadius < 2.0f)
	//	mCameraRadius = 2.0f;
}

bool Display(float timeDelta)
{
	if (Device)
	{
		//
		// Update: Rotate the mesh.
		//

		static float y = 0.0f;
		D3DXMATRIX yRot;
		D3DXMatrixRotationY(&yRot, y);
		y += timeDelta;

		if (y >= 6.28f)
			y = 0.0f;

		D3DXMATRIX World = yRot;

		//Device->SetTransform(D3DTS_VIEW, &World);


		//
		// Render
		//

		HandleKeys(timeDelta);

		SetCamera();

		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
		Device->BeginScene();

		buildObjectWorldTransforms();
		D3DXMATRIX mat;
		D3DXMatrixScaling(&mat, 10.0f, 10.0f, 10.0f);


		for (int j = 0; j < NUM_OF_MESH; j++) {
			float s = Body[j].size;
			D3DXMatrixScaling(&mat, s, s, s);
			World = mat * Body[j].toWorldXForm;
			for (int i = 0; i < Mtrls[j].size(); i++)
			{
				Device->SetTransform(D3DTS_WORLD, &World);
				Device->SetMaterial(&Mtrls[j][i]);
				Device->SetTexture(0, Textures[j][i]);
				//if(j>10)
				Mesh[j]->DrawSubset(i);
			}
		}
		Device->EndScene();
		Device->Present(0, 0, 0, 0);
	}
	return true;
}


void buildObjectWorldTransforms()
{
	// First, construct the transformation matrix that transforms
	// the ith bone into the coordinate system of its parent.

	D3DXMATRIX Rx, Ry, Rz, T;
	D3DXVECTOR3 p;
	for (int i = 0; i < NUM_OF_MESH + 1; ++i)
	{
		p = Body[i].pos;
		D3DXMatrixRotationX(&Rx, Body[i].xAngle);
		D3DXMatrixRotationY(&Ry, Body[i].yAngle);
		D3DXMatrixRotationZ(&Rz, Body[i].zAngle);
		D3DXMatrixTranslation(&T, p.x, p.y, p.z);
		Body[i].toParentXForm = Rx * Ry * Rz * T;
	}

	// For each object...
	for (int i = 0; i < NUM_OF_MESH + 1; ++i)
	{
		// Initialize to identity matrix.
		D3DXMatrixIdentity(&Body[i].toWorldXForm);

		// The ith object's world transform is given by its 
		// to-parent transform, followed by its parent's to-parent transform, 
		// followed by its grandparent's to-parent transform, and
		// so on, up to the root's to-parent transform.
		int k = i;
		while (k != -1)
		{
			Body[i].toWorldXForm *= Body[k].toParentXForm;
			k = Body[k].parent;
		}
	}
}








//
// WndProc
//
LRESULT CALLBACK d3d::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			::DestroyWindow(hwnd);
		break;
	}
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

//
// WinMain
//
int WINAPI WinMain(HINSTANCE hinstance,
	HINSTANCE prevInstance,
	PSTR cmdLine,
	int showCmd)
{
	if (!d3d::InitD3D(hinstance,
		Width, Height, true, D3DDEVTYPE_HAL, &Device))
	{
		::MessageBox(0, "InitD3D() - FAILED", 0, 0);
		return 0;
	}

	if (!Setup())
	{
		::MessageBox(0, "Setup() - FAILED", 0, 0);
		return 0;
	}

	d3d::EnterMsgLoop(Display);

	Cleanup();

	Device->Release();

	return 0;
}


