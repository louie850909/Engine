#include "model.h"
#include <functional>

struct bone_influence
{
	uint32_t bone_index;
	float bone_weight;
};
using bone_influences_per_control_point = std::vector<bone_influence>;

void fetch_bone_influences(const FbxMesh* fbx_mesh, std::vector<bone_influences_per_control_point>& bone_influences)
{
	const int control_points_count{ fbx_mesh->GetControlPointsCount() };
	bone_influences.resize(control_points_count);

	const int skin_count{ fbx_mesh->GetDeformerCount(FbxDeformer::eSkin) };
	for (int skin_index = 0; skin_index < skin_count; ++skin_index)
	{
		const FbxSkin* fbx_skin{ static_cast<FbxSkin*>(fbx_mesh->GetDeformer(skin_index, FbxDeformer::eSkin)) };

		const int cluster_count{ fbx_skin->GetClusterCount() };
		for (int cluster_index = 0; cluster_index < cluster_count; ++cluster_index)
		{
			const FbxCluster* fbx_cluster{ fbx_skin->GetCluster(cluster_index) };

			const int control_point_indices_count{ fbx_cluster->GetControlPointIndicesCount() };
			for (int control_point_indices_index = 0; control_point_indices_index < control_point_indices_count; ++control_point_indices_index)
			{
				int control_point_index{ fbx_cluster->GetControlPointIndices()[control_point_indices_index] };
				double control_point_weight
				{ fbx_cluster->GetControlPointWeights()[control_point_indices_index] };
				bone_influence& bone_influence{ bone_influences.at(control_point_index).emplace_back() };
				bone_influence.bone_index = static_cast<uint32_t>(cluster_index);
				bone_influence.bone_weight = static_cast<float>(control_point_weight);
			}
		}
	}
}

HRESULT make_dummy_texture(ID3D11Device* device, ID3D11ShaderResourceView** shader_resource_view, DWORD value, UINT dimension)
{
	HRESULT hr{ S_OK };

	D3D11_TEXTURE2D_DESC texture2d_desc{};
	texture2d_desc.Width = dimension;
	texture2d_desc.Height = dimension;
	texture2d_desc.MipLevels = 1;
	texture2d_desc.ArraySize = 1;
	texture2d_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.SampleDesc.Quality = 0;
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
	texture2d_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	size_t texels = dimension * dimension;
	unique_ptr<DWORD[]> sysmem{ make_unique< DWORD[]>(texels) };
	for (size_t i = 0; i < texels; ++i)
	{
		sysmem[i] = value;
	}
	D3D11_SUBRESOURCE_DATA subresource_data{};
	subresource_data.pSysMem = sysmem.get();
	subresource_data.SysMemPitch = sizeof(DWORD) * dimension;

	ComPtr<ID3D11Texture2D> texture2d;
	hr = device->CreateTexture2D(&texture2d_desc, &subresource_data, &texture2d);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc{};
	shader_resource_view_desc.Format = texture2d_desc.Format;
	shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shader_resource_view_desc.Texture2D.MipLevels = 1;
	hr = device->CreateShaderResourceView(texture2d.Get(), &shader_resource_view_desc, shader_resource_view);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	return hr;
}

static_mesh::static_mesh(ID3D11Device* device, const wchar_t* obj_filename, XMFLOAT3 pos, XMFLOAT3 scl, XMFLOAT3 rot, bool texture_invert)
{
	HRESULT hr = { S_OK };

	position = pos;
	scale = scl;
	rotation = rot;

	uint32_t current_index{ 0 };
	
	std::vector<XMFLOAT3> positions;
	std::vector<XMFLOAT3> normals;
	std::vector<XMFLOAT2> texcoords;
	std::vector<wstring> mtl_filenames;
	
	std::wifstream fin(obj_filename);
	_ASSERT_EXPR(fin, L"'OBJ file not found.");
	wchar_t command[256];
	while (fin)
	{
		fin >> command;
		if (0 == wcscmp(command, L"v"))
		{
			float x, y, z;
			fin >> x >> y >> z;
			b_box.min.x = min(b_box.min.x, x);
			b_box.min.y = min(b_box.min.y, y);
			b_box.min.z = min(b_box.min.z, z);
			b_box.max.x = max(b_box.max.x, x);
			b_box.max.y = max(b_box.max.y, y);
			b_box.max.z = max(b_box.max.z, z);
			positions.push_back({ x, y, z });
			fin.ignore(1024, L'\n');
		}
		else if (0 == wcscmp(command, L"vn"))
		{
			float i, j, k;
			fin >> i >> j >> k;
			normals.push_back({ i, j, k });
			fin.ignore(1024, L'\n');
		}
		else if (0 == wcscmp(command, L"vt"))
		{
			float u, v;
			fin >> u >> v;
			if(!texture_invert)
			texcoords.push_back({ u, v });
			else
			texcoords.push_back({ u, 1.0f - v });
			fin.ignore(1024, L'\n');
		}
		else if (0 == wcscmp(command, L"f"))
		{
			for (size_t i = 0; i < 3; i++)
			{
				vertex vertex;
				size_t v, vt, vn;
				fin >> v;
				vertex.position = positions.at(v - 1);
				if (L'/' == fin.peek())
				{
					fin.ignore(1);
					if (L'/' != fin.peek())
					{
						fin >> vt;
						vertex.texcoord = texcoords.at(vt - 1);
					}
					if (L'/' == fin.peek())
					{
						fin.ignore(1);
						fin >> vn;
						vertex.normal = normals.at(vn - 1);
					}
				}
				vertices.push_back(vertex);
				indices.push_back(current_index++);
			}
			fin.ignore(1024, L'\n');
		}
		else if (0 == wcscmp(command, L"mtllib"))
		{
			wchar_t mtllib[256];
			fin >> mtllib;
			mtl_filenames.push_back(mtllib);
		}
		else if (0 == wcscmp(command, L"usemtl"))
		{
			wchar_t usemtl[MAX_PATH]{ 0 };
			fin >> usemtl;
			subsets.push_back({ usemtl, static_cast<uint32_t>(indices.size()), 0 });
		}

		else
		{
			fin.ignore(1024, L'\n');
		}
	}

	fin.close();

	std::vector<subset>::reverse_iterator iterator = subsets.rbegin();
	iterator->index_count = static_cast<uint32_t>(indices.size()) - iterator->index_start;
	for (iterator = subsets.rbegin() + 1; iterator != subsets.rend(); ++iterator)
	{
		iterator->index_count = (iterator - 1)->index_start - iterator->index_start;
	}

	std::filesystem::path mtl_filename(obj_filename);
	mtl_filename.replace_filename(std::filesystem::path(mtl_filenames[0]).filename());
	
	fin.open(mtl_filename);
	//_ASSERT_EXPR(fin, L"'MTL file not found.");

	while (fin)
	{
		fin >> command;
		if (0 == wcscmp(command, L"map_Kd"))
		{
			fin.ignore();
			wchar_t map_Kd[256];
			fin >> map_Kd;
			
			std::filesystem::path path(obj_filename);
			path.replace_filename(std::filesystem::path(map_Kd).filename());
			//texture_filename = path;
			//materials.rbegin()->texture_filename = path;
			materials.rbegin()->texture_filenames[0] = path;
			fin.ignore(1024, L'\n');
			
		}
		else if (0 == wcscmp(command, L"map_bump") || 0 == wcscmp(command, L"bump"))
		{
			fin.ignore();
			wchar_t map_bump[256];
			fin >> map_bump;
			std::filesystem::path path(obj_filename);
			path.replace_filename(std::filesystem::path(map_bump).filename());
			materials.rbegin()->texture_filenames[1] = path;
			fin.ignore(1024, L'\n');
		}


		else if (0 == wcscmp(command, L"newmtl"))
		{
			fin.ignore();
			wchar_t newmtl[256];
			material material;
			fin >> newmtl;
			material.name = newmtl;
			materials.push_back(material);
		}
		else if (0 == wcscmp(command, L"Kd"))
		{
			float r, g, b;
			fin >> r >> g >> b;
			materials.rbegin()->Kd = { r, g, b, 1 };
			fin.ignore(1024, L'\n');
		}
		else
		{
			fin.ignore(1024, L'\n');
		}
	}
	fin.close();

	// MTLがない場合はデフォルトのマテリアルを作成する
	D3D11_TEXTURE2D_DESC texture2d_desc{};
	if (materials.size() == 0)
	{
		for (const subset& subset : subsets)
		{
			materials.push_back({ subset.usemtl });
		}

		for (material& material : materials)
		{
			hr = make_dummy_texture(device, material.shader_resource_views[0].GetAddressOf(), 0xFFFFFFFF, 16);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			hr = make_dummy_texture(device, material.shader_resource_views[1].GetAddressOf(), 0xFFFF7F7F, 16);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
	}
	else
	{
		for (material& material : materials)
		{
			if (material.texture_filenames[0].empty())
			{
				hr = make_dummy_texture(device, material.shader_resource_views[0].GetAddressOf(), 0xFFFFFFFF, 16);
				_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			}
			else
			{
				hr = load_texture_from_file(device, material.texture_filenames[0].c_str(), material.shader_resource_views[0].GetAddressOf(), &texture2d_desc);
				_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			}
			if (material.texture_filenames[1].empty())
			{
				hr = make_dummy_texture(device, material.shader_resource_views[1].GetAddressOf(), 0xFFFF7F7F, 16);
				_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			}
			else
			{
				hr = load_texture_from_file(device, material.texture_filenames[1].c_str(), material.shader_resource_views[1].GetAddressOf(), &texture2d_desc);
				_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			}
		}
	}

	create_com_buffers(device, vertices.data(), vertices.size(), indices.data(), indices.size());

	D3D11_INPUT_ELEMENT_DESC input_element_desc[]
	{
		{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA , 0},
		{"TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA , 0},
	};

	create_vs_from_cso(device, "static_mesh_vs.cso", vertex_shader.GetAddressOf(),
		input_layout.GetAddressOf(), input_element_desc, _countof(input_element_desc));
	create_ps_from_cso(device, "static_mesh_ps.cso", pixel_shader.GetAddressOf());

	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.ByteWidth = sizeof(constants);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//　バウンディングボックスの頂点を作成する
	DirectX::XMFLOAT3 bbox_vertices[8] =
	{
		{b_box.min.x, b_box.min.y, b_box.min.z},
		{b_box.min.x, b_box.min.y, b_box.max.z},
		{b_box.min.x, b_box.max.y, b_box.min.z},
		{b_box.min.x, b_box.max.y, b_box.max.z},
		{b_box.max.x, b_box.min.y, b_box.min.z},
		{b_box.max.x, b_box.min.y, b_box.max.z},
		{b_box.max.x, b_box.max.y, b_box.min.z},
		{b_box.max.x, b_box.max.y, b_box.max.z},
	};

	WORD bbox_indices[24] =
	{
		0,1, 1,3, 3,2, 2,0, // Back face
		4,5, 5,7, 7,6, 6,4, // Front face
		0,4, 1,5, 2,6, 3,7, // Side faces
	};

	D3D11_BUFFER_DESC vertex_buffer_desc{};
	D3D11_SUBRESOURCE_DATA subresource_data{};
	vertex_buffer_desc.ByteWidth = sizeof(bbox_vertices);
	vertex_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertex_buffer_desc.CPUAccessFlags = 0;
	vertex_buffer_desc.MiscFlags = 0;
	vertex_buffer_desc.StructureByteStride = 0;

	subresource_data.pSysMem = bbox_vertices;
	subresource_data.SysMemPitch = 0;
	subresource_data.SysMemSlicePitch = 0;

	hr = device->CreateBuffer(&vertex_buffer_desc, &subresource_data, bbox_vertex_buffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	D3D11_BUFFER_DESC index_buffer_desc{};
	index_buffer_desc.ByteWidth = sizeof(bbox_indices);
	index_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	index_buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	index_buffer_desc.CPUAccessFlags = 0;
	index_buffer_desc.MiscFlags = 0;
	index_buffer_desc.StructureByteStride = 0;

	subresource_data.pSysMem = bbox_indices;

	hr = device->CreateBuffer(&index_buffer_desc, &subresource_data, bbox_index_buffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	D3D11_INPUT_ELEMENT_DESC b_box_input_element_desc[]
	{
		{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	create_vs_from_cso(device, "bbox_vs.cso", bbox_vertex_shader.GetAddressOf(),
				bbox_input_layout.GetAddressOf(), b_box_input_element_desc, _countof(b_box_input_element_desc));
	create_ps_from_cso(device, "bbox_ps.cso", bbox_pixel_shader.GetAddressOf());

	XMMATRIX w, s, r, t;
	w = XMMatrixIdentity();
	s = XMMatrixScaling(scale.x, scale.y, scale.z);
	w = s * w;
	r = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y + XM_PI, rotation.z);
	w = r * w;
	t = XMMatrixTranslation(position.x, position.y, position.z);
	w = t * w;
	world_matrix = w;
}

static_mesh::~static_mesh()
{
	delete bbox_vertex_shader.Get();
	delete bbox_pixel_shader.Get();
	delete bbox_vertex_buffer.Get();
	delete bbox_index_buffer.Get();
	delete bbox_input_layout.Get();
	delete vertex_shader.Get();
	delete pixel_shader.Get();
	delete vertex_buffer.Get();
	delete index_buffer.Get();
	delete input_layout.Get();
	delete constant_buffer.Get();
	vertices.clear();
	indices.clear();
}

void static_mesh::draw(render Render)
{
	/*XMMATRIX world, scl, rot, trans;
	world = XMMatrixIdentity();
	scl = XMMatrixScaling(scale.x, scale.y, scale.z);
	world = scl * world;
	rot = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y + XM_PI, rotation.z);
	world = rot * world;
	trans = XMMatrixTranslation(position.x, position.y, position.z);
	world = trans * world;
	world_matrix = world;*/

	uint32_t stride{ sizeof(vertex) };
	uint32_t offset{ 0 };
	Render.get_immediate_context()->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
	Render.get_immediate_context()->IASetIndexBuffer(index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	Render.get_immediate_context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Render.get_immediate_context()->IASetInputLayout(input_layout.Get());

	Render.get_immediate_context()->VSSetShader(vertex_shader.Get(), nullptr, 0);
	Render.get_immediate_context()->PSSetShader(pixel_shader.Get(), nullptr, 0);

	// シェーダー リソースのバインド
	for (const material& material : materials)
	{
		Render.get_immediate_context()->PSSetShaderResources(0, 1, material.shader_resource_views[0].GetAddressOf());
		Render.get_immediate_context()->PSSetShaderResources(1, 1, material.shader_resource_views[1].GetAddressOf());

		XMStoreFloat4x4(&constant_data.world, world_matrix);
		XMFLOAT4 material_color = { 1,1,1,1 };
		XMStoreFloat4(&constant_data.material_color, XMLoadFloat4(&material_color) * XMLoadFloat4(&material.Kd));

		Render.get_immediate_context()->UpdateSubresource(constant_buffer.Get(), 0, 0, &constant_data, 0, 0);
		Render.get_immediate_context()->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());

		for (const subset& subset : subsets)
		{
			if (material.name == subset.usemtl)
			{
				Render.get_immediate_context()->DrawIndexed(subset.index_count, subset.index_start, 0);
			}
		}
	}
}

void static_mesh::drawBBox(render Render)
{
	XMMATRIX world, scl, rot, trans;
	world = XMMatrixIdentity();
	scl = XMMatrixScaling(scale.x, scale.y, scale.z);
	world = scl * world;
	rot = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y + XM_PI, rotation.z);
	world = rot * world;
	trans = XMMatrixTranslation(position.x, position.y, position.z);
	world = trans * world;

	uint32_t stride{ sizeof(XMFLOAT3) };
	uint32_t offset{ 0 };
	Render.get_immediate_context()->IASetVertexBuffers(0, 1, bbox_vertex_buffer.GetAddressOf(), &stride, &offset);
	Render.get_immediate_context()->IASetIndexBuffer(bbox_index_buffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	Render.get_immediate_context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	Render.get_immediate_context()->IASetInputLayout(bbox_input_layout.Get());

	Render.get_immediate_context()->VSSetShader(bbox_vertex_shader.Get(), nullptr, 0);
	Render.get_immediate_context()->PSSetShader(bbox_pixel_shader.Get(), nullptr, 0);

	XMStoreFloat4x4(&constant_data.world, world);
	XMFLOAT4 material_color = { 1,1,1,1 };
	XMStoreFloat4(&constant_data.material_color, XMLoadFloat4(&material_color));

	Render.get_immediate_context()->UpdateSubresource(constant_buffer.Get(), 0, 0, &constant_data, 0, 0);
	Render.get_immediate_context()->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());

	Render.set_rasterizer_state(render::RASTERIZER_WIREFRAME);
	Render.get_immediate_context()->DrawIndexed(24, 0, 0);
	Render.set_rasterizer_state(render::RASTERIZER_NORMAL);
}

void static_mesh::update()
{
}

void static_mesh::create_com_buffers(ID3D11Device* device, vertex* vertices, size_t vertex_count, uint32_t* indices, size_t index_count)
{
	HRESULT hr{ S_OK };

	D3D11_BUFFER_DESC buffer_desc{};
	D3D11_SUBRESOURCE_DATA subresource_data{};
	buffer_desc.ByteWidth = static_cast<UINT>(sizeof(vertex) * vertex_count);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;

	subresource_data.pSysMem = vertices;
	subresource_data.SysMemPitch = 0;
	subresource_data.SysMemSlicePitch = 0;

	hr = device->CreateBuffer(&buffer_desc, &subresource_data, &vertex_buffer);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	buffer_desc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * index_count);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	subresource_data.pSysMem = indices;

	hr = device->CreateBuffer(&buffer_desc, &subresource_data, &index_buffer);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

skinned_mesh::skinned_mesh(ID3D11Device* device, const char* fbx_filename, bool triangulate, XMFLOAT3 pos, XMFLOAT3 scl, XMFLOAT3 rot, float sampling_rate)
{
	position = pos;
	scale = scl;
	rotation = rot;

	std::filesystem::path cereal_filename(fbx_filename);
	cereal_filename.replace_extension("cereal");
	if (std::filesystem::exists(cereal_filename.c_str()))
	{
		std::ifstream ifs(cereal_filename.c_str(), std::ios::binary);
		cereal::BinaryInputArchive deserialization(ifs);
		deserialization(scene_view, meshes, materials, animation_clips);
	}
	else
	{
		FbxManager* fbx_manager{ FbxManager::Create() };
		FbxScene* fbx_scene{ FbxScene::Create(fbx_manager, "") };

		FbxImporter* fbx_importer{ FbxImporter::Create(fbx_manager, "") };
		bool import_status{ false };
		import_status = fbx_importer->Initialize(fbx_filename);
		_ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());

		import_status = fbx_importer->Import(fbx_scene);
		_ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());

		FbxGeometryConverter fbx_converter(fbx_manager);
		if (triangulate)
		{
			fbx_converter.Triangulate(fbx_scene, true/*replace*/, false/*legacy*/);
			fbx_converter.RemoveBadPolygonsFromMeshes(fbx_scene);
		}

		std::function<void(FbxNode*)> traverse{ [&](FbxNode* fbx_node)
			{
				scene::node& node{ scene_view.nodes.emplace_back() };
				node.attribute = fbx_node->GetNodeAttribute() ? fbx_node->GetNodeAttribute()->GetAttributeType() : FbxNodeAttribute::EType::eUnknown;
				node.name = fbx_node->GetName();
				node.unique_id = fbx_node->GetUniqueID();
				node.parent_index = scene_view.indexof(fbx_node->GetParent() ? fbx_node->GetParent()->GetUniqueID() : 0);
				for (int child_index = 0; child_index < fbx_node->GetChildCount(); ++child_index)
				{
					traverse(fbx_node->GetChild(child_index));
				}
		}};
		traverse(fbx_scene->GetRootNode());

		fetch_meshes(fbx_scene, meshes);
		fetch_materials(fbx_scene, materials);
		fetch_animation(fbx_scene, animation_clips, sampling_rate);

		fbx_manager->Destroy();

		std::ofstream ofs(cereal_filename.c_str(), std::ios::binary);
		cereal::BinaryOutputArchive serialization(ofs);
		serialization(scene_view, meshes, materials, animation_clips);
	}

	create_com_objects(device, fbx_filename);
}

skinned_mesh::skinned_mesh(ID3D11Device* device, const char* fbx_filename, bool triangulate, XMFLOAT3 pos, XMFLOAT3 scl, XMFLOAT3 rot)
{
	skinned_mesh(device, fbx_filename, triangulate, pos, scl, rot, 0.0f);
}

skinned_mesh::~skinned_mesh()
{
	for (mesh& mesh : meshes)
	{
		mesh.vertex_buffer->Release();
		mesh.index_buffer->Release();
		mesh.bbox_index_buffer->Release();
		mesh.bbox_vertex_buffer->Release();
	}

	for (int i = 0; i < materials.size(); i++)
	{
		materials.at(i).shader_resource_views[0]->Release();
		materials.at(i).shader_resource_views[1]->Release();
		materials.at(i).shader_resource_views[2]->Release();
		materials.at(i).shader_resource_views[3]->Release();
	}

	animation_clips.clear();
	bbox_input_layout->Release();
	bbox_vertex_shader->Release();
	bbox_pixel_shader->Release();
}

void skinned_mesh::fetch_meshes(FbxScene* fbx_scene, std::vector<mesh>& meshes)
{
	for (const scene::node& node : scene_view.nodes)
	{
		if (node.attribute != FbxNodeAttribute::EType::eMesh)
		{
			continue;
		}

		FbxNode* fbx_node{ fbx_scene->FindNodeByName(node.name.c_str()) };
		FbxMesh * fbx_mesh{ fbx_node->GetMesh() };
		
		mesh & mesh{ meshes.emplace_back() };
		mesh.unique_id = fbx_mesh->GetNode()->GetUniqueID();
		mesh.name = fbx_mesh->GetNode()->GetName();
		mesh.node_index = scene_view.indexof(mesh.unique_id);
		mesh.default_global_transform = to_xmfloat4x4(fbx_mesh->GetNode()->EvaluateGlobalTransform());

		std::vector<bone_influences_per_control_point> bone_influences;
		fetch_bone_influences(fbx_mesh, bone_influences);
		fetch_skeleton(fbx_mesh, mesh.bind_pose);

		std::vector<mesh::subset>& subsets{ mesh.subsets };
		const int material_count{ fbx_mesh->GetNode()->GetMaterialCount() };
		subsets.resize(material_count > 0 ? material_count : 1);
		for (int material_index = 0; material_index < material_count; ++material_index)
		{
			const FbxSurfaceMaterial * fbx_material{ fbx_mesh->GetNode()->GetMaterial(material_index) };
			subsets.at(material_index).material_name = fbx_material->GetName();
			subsets.at(material_index).material_unique_id = fbx_material->GetUniqueID();
		}
		if (material_count > 0)
		{
			const int polygon_count{ fbx_mesh->GetPolygonCount() };
			for (int polygon_index = 0; polygon_index < polygon_count; ++polygon_index)
			{
				const int material_index{ fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(polygon_index) };
				subsets.at(material_index).index_count += 3;
			}
			uint32_t offset{ 0 };
			for (mesh::subset& subset : subsets)
			{
				subset.start_index_location = offset;
				offset += subset.index_count;
				// This will be used as counter in the following procedures, reset to zero
				subset.index_count = 0;
			}
		}
		
		const int polygon_count{ fbx_mesh->GetPolygonCount() };
		mesh.vertices.resize(polygon_count * 3LL);
		mesh.indices.resize(polygon_count * 3LL);
		
		FbxStringList uv_names;
		fbx_mesh->GetUVSetNames(uv_names);
		const FbxVector4 * control_points{ fbx_mesh->GetControlPoints() };
		for (int polygon_index = 0; polygon_index < polygon_count; ++polygon_index)
		{
			const int material_index{ material_count > 0 ? fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(polygon_index) : 0 };
			mesh::subset& subset{ subsets.at(material_index) };
			const uint32_t offset{ subset.start_index_location + subset.index_count };

			for (int position_in_polygon = 0; position_in_polygon < 3; ++position_in_polygon)
			{
				const int vertex_index{ polygon_index * 3 + position_in_polygon };
				
				vertex vertex;
				const int polygon_vertex{ fbx_mesh->GetPolygonVertex(polygon_index, position_in_polygon) };
				vertex.position.x = static_cast<float>(control_points[polygon_vertex][0]);
				vertex.position.y = static_cast<float>(control_points[polygon_vertex][1]);
				vertex.position.z = static_cast<float>(control_points[polygon_vertex][2]);

				const bone_influences_per_control_point& influences_per_control_point{ bone_influences.at(polygon_vertex) };
				for (size_t influence_index = 0; influence_index < influences_per_control_point.size(); ++influence_index)
				{
					//if (influence_index < MAX_BONE_INFLUENCES)
					{
						vertex.bone_weights[influence_index] = influences_per_control_point.at(influence_index).bone_weight;
						vertex.bone_indices[influence_index] = influences_per_control_point.at(influence_index).bone_index;
					}
				}

				if (fbx_mesh->GetElementNormalCount() > 0)
				{
					FbxVector4 normal;
					fbx_mesh->GetPolygonVertexNormal(polygon_index, position_in_polygon, normal);
					vertex.normal.x = static_cast<float>(normal[0]);
					vertex.normal.y = static_cast<float>(normal[1]);
					vertex.normal.z = static_cast<float>(normal[2]);
				}
				if (fbx_mesh->GetElementUVCount() > 0)
				{
					FbxVector2 uv;
					bool unmapped_uv;
					fbx_mesh->GetPolygonVertexUV(polygon_index, position_in_polygon,uv_names[0], uv, unmapped_uv);
					vertex.texcoord.x = static_cast<float>(uv[0]);
					vertex.texcoord.y = 1.0f - static_cast<float>(uv[1]);
				}
				if (fbx_mesh->GenerateTangentsData(0, false))
				{
					const FbxGeometryElementTangent* tangent = fbx_mesh->GetElementTangent(0);
					vertex.tangent.x = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[0]);
					vertex.tangent.y = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[1]);
					vertex.tangent.z = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[2]);
					vertex.tangent.w = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[3]);
				}

				mesh.vertices.at(vertex_index) = std::move(vertex);
				mesh.indices.at(static_cast<size_t>(offset) + position_in_polygon) = vertex_index;
				subset.index_count++;
			}
		}

		for (const vertex& v : mesh.vertices)
		{
			mesh.bounding_box[0].x = std::min<float>(mesh.bounding_box[0].x, v.position.x);
			mesh.bounding_box[0].y = std::min<float>(mesh.bounding_box[0].y, v.position.y);
			mesh.bounding_box[0].z = std::min<float>(mesh.bounding_box[0].z, v.position.z);
			mesh.bounding_box[1].x = std::max<float>(mesh.bounding_box[1].x, v.position.x);
			mesh.bounding_box[1].y = std::max<float>(mesh.bounding_box[1].y, v.position.y);
			mesh.bounding_box[1].z = std::max<float>(mesh.bounding_box[1].z, v.position.z);
		}
	}
}

void skinned_mesh::fetch_materials(FbxScene* fbx_scene, std::unordered_map<uint64_t, material>& materials)
{
	const size_t node_count{ scene_view.nodes.size() };
	for (size_t node_index = 0; node_index < node_count; ++node_index)
	{
		const scene::node & node{ scene_view.nodes.at(node_index) };
		const FbxNode * fbx_node{ fbx_scene->FindNodeByName(node.name.c_str()) };
		
		const int material_count{ fbx_node->GetMaterialCount() };

		if (material_count == 0)
		{
			make_dummy_material(materials);
		}
		
		for (int material_index = 0; material_index < material_count; ++material_index)
		{
			const FbxSurfaceMaterial * fbx_material{ fbx_node->GetMaterial(material_index) };
			
			material material;
			material.name = fbx_material->GetName();
			material.unique_id = fbx_material->GetUniqueID();
			FbxProperty fbx_property;
			fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sDiffuse);
			if (fbx_property.IsValid())
			{
				const FbxDouble3 color{ fbx_property.Get<FbxDouble3>() };
				material.Kd.x = static_cast<float>(color[0]);
				material.Kd.y = static_cast<float>(color[1]);
				material.Kd.z = static_cast<float>(color[2]);
				material.Kd.w = 1.0f;

				const FbxFileTexture* fbx_texture{ fbx_property.GetSrcObject<FbxFileTexture>() };
				material.texture_filenames[0] = fbx_texture ? fbx_texture->GetRelativeFileName() : "";
			}
			
			fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sSpecular);
			if (fbx_property.IsValid())
			{
				const FbxDouble3 color{ fbx_property.Get<FbxDouble3>() };
				material.Ks.x = static_cast<float>(color[0]);
				material.Ks.y = static_cast<float>(color[1]);
				material.Ks.z = static_cast<float>(color[2]);
				material.Ks.w = 1.0f;

				const FbxFileTexture* fbx_texture{ fbx_property.GetSrcObject<FbxFileTexture>() };
				material.texture_filenames[1] = fbx_texture ? fbx_texture->GetRelativeFileName() : "";
			}

			fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sAmbient);
			if (fbx_property.IsValid())
			{
				const FbxDouble3 color{ fbx_property.Get<FbxDouble3>() };
				material.Ka.x = static_cast<float>(color[0]);
				material.Ka.y = static_cast<float>(color[1]);
				material.Ka.z = static_cast<float>(color[2]);
				material.Ka.w = 1.0f;

				const FbxFileTexture* fbx_texture{ fbx_property.GetSrcObject<FbxFileTexture>() };
				material.texture_filenames[2] = fbx_texture ? fbx_texture->GetRelativeFileName() : "";
			}

			fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sNormalMap);
			if (fbx_property.IsValid())
			{
				const FbxFileTexture * file_texture{ fbx_property.GetSrcObject<FbxFileTexture>() };
				material.texture_filenames[1] = file_texture ? file_texture->GetRelativeFileName() : "";
			}

			materials.emplace(material.unique_id, std::move(material));
		}
	}
}

void skinned_mesh::fetch_skeleton(FbxMesh* fbx_mesh, skeleton& bind_pose)
{
	const int deformer_count = fbx_mesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int deformer_index = 0; deformer_index < deformer_count; ++deformer_index)
	{
		FbxSkin * skin = static_cast<FbxSkin*>(fbx_mesh->GetDeformer(deformer_index, FbxDeformer::eSkin));
		const int cluster_count = skin->GetClusterCount();
		bind_pose.bones.resize(cluster_count);
		for (int cluster_index = 0; cluster_index < cluster_count; ++cluster_index)
		{
			FbxCluster * cluster = skin->GetCluster(cluster_index);
			
			skeleton::bone & bone{ bind_pose.bones.at(cluster_index) };
			bone.name = cluster->GetLink()->GetName();
			bone.unique_id = cluster->GetLink()->GetUniqueID();
			bone.parent_index = bind_pose.indexof(cluster->GetLink()->GetParent()->GetUniqueID());
			bone.node_index = scene_view.indexof(bone.unique_id);
			
			//'reference_global_init_position' is used to convert from local space of model(mesh) to global space of scene.
			FbxAMatrix reference_global_init_position;
			cluster->GetTransformMatrix(reference_global_init_position);
			
			// 'cluster_global_init_position' is used to convert from local space of bone to global space of scene.
			FbxAMatrix cluster_global_init_position;
			cluster->GetTransformLinkMatrix(cluster_global_init_position);
			
			// Matrices are defined using the Column Major scheme. When a FbxAMatrix represents a transformation
			// (translation, rotation and scale), the last row of the matrix represents the translation part of the transformation.
			// Compose 'bone.offset_transform' matrix that trnasforms position from mesh space to bone space.
			// This matrix is called the offset matrix.
			bone.offset_transform = to_xmfloat4x4(cluster_global_init_position.Inverse() * reference_global_init_position);
		}
	}
}

void skinned_mesh::fetch_animation(FbxScene* fbx_scene, std::vector<animation>& animation_clips, float sampling_rate)
{
	FbxArray<FbxString*> animation_stack_names;
	fbx_scene->FillAnimStackNameArray(animation_stack_names);
	const int animation_stack_count{ animation_stack_names.GetCount() };
	for (int animation_stack_index = 0; animation_stack_index < animation_stack_count; ++animation_stack_index)
	{
		animation & animation_clip{ animation_clips.emplace_back() };
		animation_clip.name = animation_stack_names[animation_stack_index]->Buffer();
		
		FbxAnimStack * animation_stack{ fbx_scene->FindMember<FbxAnimStack>(animation_clip.name.c_str()) };
		fbx_scene->SetCurrentAnimationStack(animation_stack);
		
		const FbxTime::EMode time_mode{ fbx_scene->GetGlobalSettings().GetTimeMode() };
		FbxTime one_second;
		one_second.SetTime(0, 0, 1, 0, 0, time_mode);
		animation_clip.sampling_rate = sampling_rate > 0 ? sampling_rate : static_cast<float>(one_second.GetFrameRate(time_mode));
		const FbxTime sampling_interval{ static_cast<FbxLongLong>(one_second.Get() / animation_clip.sampling_rate) };
		const FbxTakeInfo * take_info{ fbx_scene->GetTakeInfo(animation_clip.name.c_str()) };
		const FbxTime start_time{ take_info->mLocalTimeSpan.GetStart() };
		const FbxTime stop_time{ take_info->mLocalTimeSpan.GetStop() };
		for (FbxTime time = start_time; time < stop_time; time += sampling_interval)
		{
			animation::keyframe & keyframe{ animation_clip.sequence.emplace_back() };
			
			const size_t node_count{ scene_view.nodes.size() };
			keyframe.nodes.resize(node_count);
			for (size_t node_index = 0; node_index < node_count; ++node_index)
			{
				FbxNode* fbx_node{ fbx_scene->FindNodeByName(scene_view.nodes.at(node_index).name.c_str()) };
				if (fbx_node)
				{
					animation::keyframe::node & node{ keyframe.nodes.at(node_index) };
					// 'global_transform' is a transformation matrix of a node with respect to the scene's global coordinate system.
					node.global_transform = to_xmfloat4x4(fbx_node->EvaluateGlobalTransform(time));

					// 'local_transform' is a transformation matrix of a node with respect to　its parent's local coordinate system.
					const FbxAMatrix & local_transform{ fbx_node->EvaluateLocalTransform(time) };
					node.scaling = to_xmfloat3(local_transform.GetS());
					node.rotation = to_xmfloat4(local_transform.GetQ());
					node.translation = to_xmfloat3(local_transform.GetT());
				}
			}
		}
	}
	for (int animation_stack_index = 0; animation_stack_index < animation_stack_count; ++animation_stack_index)
	{
		delete animation_stack_names[animation_stack_index];
	}
}

void skinned_mesh::make_dummy_material(std::unordered_map<uint64_t, material>& materials)
{
	material material;
	material.name = "dummy";
	material.unique_id = 0;
	material.Kd.x = 0.5f;
	material.Kd.y = 0.5f;
	material.Kd.z = 0.5f;
	material.Kd.w = 1.0f;
	material.Ks.x = 0.5f;
	material.Ks.y = 0.5f;
	material.Ks.z = 0.5f;
	material.Ks.w = 1.0f;
	material.Ka.x = 0.5f;
	material.Ka.y = 0.5f;
	material.Ka.z = 0.5f;
	material.Ka.w = 1.0f;
	material.texture_filenames[0] = "";
	material.texture_filenames[1] = "";
	material.texture_filenames[2] = "";
	materials.emplace(material.unique_id, std::move(material));
}

void skinned_mesh::draw(render Render)
{
	draw(Render, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), LHS_YUP);
}

void skinned_mesh::draw(render Render, int coordinate_system)
{
	draw(Render, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), coordinate_system);
}

void skinned_mesh::draw(render Render, XMFLOAT4 material_color, int coordinate_system)
{
	XMMATRIX world, scl, rot, trans;
	world = XMMatrixIdentity();
	scale = XMFLOAT3(XMConvertToRadians(scale.x), XMConvertToRadians(scale.y), XMConvertToRadians(scale.z));
	scl = XMMatrixScaling(scale.x, scale.y, scale.z);
	world = scl * world;
	rot = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y + XM_PI, rotation.z);
	world = rot * world;
	trans = XMMatrixTranslation(position.x, position.y, position.z);
	world = trans * world;

	const DirectX::XMFLOAT4X4 coordinate_system_transforms[]{
		{ -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },		// 0:RHS Y-UP
		{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },		// 1:LHS Y-UP
		{ -1, 0, 0, 0, 0, 0, -1, 0, 0, 1, 0, 0, 0, 0, 0, 1 },	// 2:RHS Z-UP
		{ 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1 },		// 3:LHS Z-UP
	};

	world = DirectX::XMLoadFloat4x4(&coordinate_system_transforms[coordinate_system]) * world;

	if (coordinate_system != 1)
	{
		D3D11_RASTERIZER_DESC rasterizer_desc{};
		rasterizer_desc.FillMode = D3D11_FILL_SOLID;
		rasterizer_desc.CullMode = D3D11_CULL_BACK;
		rasterizer_desc.FrontCounterClockwise = true;
		rasterizer_desc.DepthClipEnable = true;
		rasterizer_desc.ScissorEnable = false;
		rasterizer_desc.MultisampleEnable = false;
		rasterizer_desc.AntialiasedLineEnable = false;
		ID3D11RasterizerState* rasterizer_state{ nullptr };
		Render.get_device()->CreateRasterizerState(&rasterizer_desc, &rasterizer_state);
		Render.get_immediate_context()->RSSetState(rasterizer_state);
	}


	for (const mesh& mesh : meshes)
	{
		uint32_t stride{ sizeof(vertex) };
		uint32_t offset{ 0 };
		Render.get_immediate_context()->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
		Render.get_immediate_context()->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		Render.get_immediate_context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		Render.get_immediate_context()->IASetInputLayout(input_layout.Get());

		Render.get_immediate_context()->VSSetShader(vertex_shader.Get(), nullptr, 0);
		Render.get_immediate_context()->PSSetShader(pixel_shader.Get(), nullptr, 0);

		XMStoreFloat4x4(&constant_data.world, XMLoadFloat4x4(&mesh.default_global_transform) * world);

		for (const mesh::subset& subset : mesh.subsets)
		{
			const material& material{ materials.at(subset.material_unique_id) };

			XMFLOAT4 color = material_color;
			XMStoreFloat4(&constant_data.material_color, XMLoadFloat4(&color) * XMLoadFloat4(&material.Kd));

			Render.get_immediate_context()->UpdateSubresource(constant_buffer.Get(), 0, 0, &constant_data, 0, 0);
			Render.get_immediate_context()->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());

			Render.get_immediate_context()->PSSetShaderResources(0, 1, material.shader_resource_views[0].GetAddressOf());
			Render.get_immediate_context()->DrawIndexed(subset.index_count, subset.start_index_location, 0);
		}
	}

	Render.set_rasterizer_state(render::RASTERIZER_NORMAL);
}

void skinned_mesh::draw(render Render, XMFLOAT4 material_color, int coordinate_system, const animation::keyframe* keyframe)
{
	XMMATRIX world, scl, rot, trans;
	world = XMMatrixIdentity();
	scl = XMMatrixScaling(scale.x, scale.y, scale.z);
	world = XMMatrixMultiply(world, scl);
	rot = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y + XM_PI, rotation.z);
	world = XMMatrixMultiply(world, rot);
	trans = XMMatrixTranslation(position.x, position.y, position.z);
	world = XMMatrixMultiply(world, trans);

	const DirectX::XMFLOAT4X4 coordinate_system_transforms[]{
		{ -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },		// 0:RHS Y-UP
		{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },		// 1:LHS Y-UP
		{ -1, 0, 0, 0, 0, 0, -1, 0, 0, 1, 0, 0, 0, 0, 0, 1 },	// 2:RHS Z-UP
		{ 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1 },		// 3:LHS Z-UP
	};

	world = DirectX::XMLoadFloat4x4(&coordinate_system_transforms[coordinate_system]) * world;

	if (coordinate_system != 1)
	{
		D3D11_RASTERIZER_DESC rasterizer_desc{};
		rasterizer_desc.FillMode = D3D11_FILL_SOLID;
		rasterizer_desc.CullMode = D3D11_CULL_BACK;
		rasterizer_desc.FrontCounterClockwise = true;
		rasterizer_desc.DepthClipEnable = true;
		rasterizer_desc.ScissorEnable = false;
		rasterizer_desc.MultisampleEnable = false;
		rasterizer_desc.AntialiasedLineEnable = false;
		ID3D11RasterizerState* rasterizer_state{ nullptr };
		Render.get_device()->CreateRasterizerState(&rasterizer_desc, &rasterizer_state);
		Render.get_immediate_context()->RSSetState(rasterizer_state);
	}


	for (const mesh& mesh : meshes)
	{
		uint32_t stride{ sizeof(vertex) };
		uint32_t offset{ 0 };
		Render.get_immediate_context()->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
		Render.get_immediate_context()->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		Render.get_immediate_context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		Render.get_immediate_context()->IASetInputLayout(input_layout.Get());

		Render.get_immediate_context()->VSSetShader(vertex_shader.Get(), nullptr, 0);
		Render.get_immediate_context()->PSSetShader(pixel_shader.Get(), nullptr, 0);

		if (keyframe && keyframe->nodes.size() > 0)
		{
			const animation::keyframe::node& mesh_node{ keyframe->nodes.at(mesh.node_index) };
			XMStoreFloat4x4(&constant_data.world, XMLoadFloat4x4(&mesh_node.global_transform) * world);

			const size_t bone_count{ mesh.bind_pose.bones.size() };
			_ASSERT_EXPR(bone_count < MAX_BONES, L"The value of the 'bone_count' has exceeded MAX_BONES.");

			for (size_t bone_index = 0; bone_index < bone_count; ++bone_index)
			{
				const skeleton::bone & bone{ mesh.bind_pose.bones.at(bone_index) };
				const animation::keyframe::node & bone_node{ keyframe->nodes.at(bone.node_index) };
				XMStoreFloat4x4(&constant_data.bone_transforms[bone_index],
					XMLoadFloat4x4(&bone.offset_transform) *
					XMLoadFloat4x4(&bone_node.global_transform) *
					XMMatrixInverse(nullptr, XMLoadFloat4x4(&mesh.default_global_transform)));
			}
		}
		else
		{
			XMStoreFloat4x4(&constant_data.world,XMLoadFloat4x4(&mesh.default_global_transform) * world);
			for (size_t bone_index = 0; bone_index < MAX_BONES; ++bone_index)
			{
				constant_data.bone_transforms[bone_index] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
			}
		}

		for (const mesh::subset& subset : mesh.subsets)
		{
			const material& material{ materials.at(subset.material_unique_id) };

			XMFLOAT4 color = material_color;
			XMStoreFloat4(&constant_data.material_color, XMLoadFloat4(&color) * XMLoadFloat4(&material.Kd));

			Render.get_immediate_context()->UpdateSubresource(constant_buffer.Get(), 0, 0, &constant_data, 0, 0);
			Render.get_immediate_context()->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());

			Render.get_immediate_context()->PSSetShaderResources(0, 1, material.shader_resource_views[0].GetAddressOf());
			Render.get_immediate_context()->PSSetShaderResources(1, 1, material.shader_resource_views[1].GetAddressOf());

			Render.get_immediate_context()->DrawIndexed(subset.index_count, subset.start_index_location, 0);
		}
	}
#ifdef AST
	for (const mesh& mesh : meshes)
	{
		uint32_t stride{ sizeof(XMFLOAT3) };
		uint32_t offset{ 0 };
		Render.get_immediate_context()->IASetVertexBuffers(0, 1, mesh.bbox_vertex_buffer.GetAddressOf(), &stride, &offset);
		Render.get_immediate_context()->IASetIndexBuffer(mesh.bbox_index_buffer.Get(), DXGI_FORMAT_R16_UINT, 0);
		Render.get_immediate_context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		Render.get_immediate_context()->IASetInputLayout(bbox_input_layout.Get());

		Render.get_immediate_context()->VSSetShader(bbox_vertex_shader.Get(), nullptr, 0);
		Render.get_immediate_context()->PSSetShader(bbox_pixel_shader.Get(), nullptr, 0);

		if (keyframe && keyframe->nodes.size() > 0)
		{
			const animation::keyframe::node& mesh_node{ keyframe->nodes.at(mesh.node_index) };
			XMStoreFloat4x4(&constant_data.world, XMLoadFloat4x4(&mesh_node.global_transform) * world);

			const size_t bone_count{ mesh.bind_pose.bones.size() };
			_ASSERT_EXPR(bone_count < MAX_BONES, L"The value of the 'bone_count' has exceeded MAX_BONES.");

			for (size_t bone_index = 0; bone_index < bone_count; ++bone_index)
			{
				const skeleton::bone& bone{ mesh.bind_pose.bones.at(bone_index) };
				const animation::keyframe::node& bone_node{ keyframe->nodes.at(bone.node_index) };
				XMStoreFloat4x4(&constant_data.bone_transforms[bone_index],
					XMLoadFloat4x4(&bone.offset_transform) *
					XMLoadFloat4x4(&bone_node.global_transform) *
					XMMatrixInverse(nullptr, XMLoadFloat4x4(&mesh.default_global_transform)));
			}
		}
		else
		{
			XMStoreFloat4x4(&constant_data.world, XMLoadFloat4x4(&mesh.default_global_transform) * world);
			for (size_t bone_index = 0; bone_index < MAX_BONES; ++bone_index)
			{
				constant_data.bone_transforms[bone_index] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
			}
		}

		Render.get_immediate_context()->UpdateSubresource(constant_buffer.Get(), 0, 0, &constant_data, 0, 0);
		Render.get_immediate_context()->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());

		Render.set_rasterizer_state(render::RASTERIZER_WIREFRAME);
		Render.get_immediate_context()->DrawIndexed(24, 0, 0);
	}
#endif // _DEBUG
	Render.set_rasterizer_state(render::RASTERIZER_NORMAL);
}

void skinned_mesh::update_animation(animation::keyframe& keyframe)
{
	size_t node_count{ keyframe.nodes.size() };
	for (size_t node_index = 0; node_index < node_count; ++node_index)
	{
		animation::keyframe::node & node{ keyframe.nodes.at(node_index) };
		XMMATRIX S{ XMMatrixScaling(node.scaling.x, node.scaling.y, node.scaling.z) };
		XMMATRIX R{ XMMatrixRotationQuaternion(XMLoadFloat4(&node.rotation)) };
		XMMATRIX T{ XMMatrixTranslation(node.translation.x, node.translation.y, node.translation.z) };
		
		int64_t parent_index{ scene_view.nodes.at(node_index).parent_index };
		XMMATRIX P{ parent_index < 0 ? XMMatrixIdentity() :
		XMLoadFloat4x4(&keyframe.nodes.at(parent_index).global_transform) };
		
		XMStoreFloat4x4(&node.global_transform, S * R * T * P);
	}
}

bool skinned_mesh::append_animations(const char* animation_filename, float sampling_rate, int mode)
{
	std::filesystem::path cereal_filename(animation_filename);
	cereal_filename.replace_extension("cereal");
	if (std::filesystem::exists(cereal_filename.c_str()) && mode != ASCII)
	{
		std::ifstream ifs(cereal_filename.c_str(), std::ios::binary);
		cereal::BinaryInputArchive deserialization(ifs);
		deserialization( animation_clips);
	}
	else
	{
		FbxManager* fbx_manager{ FbxManager::Create() };
		FbxScene* fbx_scene{ FbxScene::Create(fbx_manager, "") };

		FbxImporter* fbx_importer{ FbxImporter::Create(fbx_manager, "") };
		bool import_status{ false };
		import_status = fbx_importer->Initialize(animation_filename);
		_ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());
		import_status = fbx_importer->Import(fbx_scene);
		_ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());

		fetch_animation(fbx_scene, animation_clips, sampling_rate);

		fbx_manager->Destroy();

		std::ofstream ofs(cereal_filename.c_str(), std::ios::binary);
		cereal::BinaryOutputArchive serialization(ofs);
		serialization( animation_clips);
	}
	
	return true;
}

void skinned_mesh::blend_animations(const animation::keyframe* keyframes[2], float factor, animation::keyframe& keyframe)
{
	size_t node_count{ keyframes[0]->nodes.size() };
	keyframe.nodes.resize(node_count);
	for (size_t node_index = 0; node_index < node_count; ++node_index)
	{
		XMVECTOR S[2]{ XMLoadFloat3(&keyframes[0]->nodes.at(node_index).scaling), XMLoadFloat3(&keyframes[1]->nodes.at(node_index).scaling) };
		XMStoreFloat3(&keyframe.nodes.at(node_index).scaling, XMVectorLerp(S[0], S[1], factor));
		
		XMVECTOR R[2]{ XMLoadFloat4(&keyframes[0]->nodes.at(node_index).rotation), XMLoadFloat4(&keyframes[1]->nodes.at(node_index).rotation) };
		XMStoreFloat4(&keyframe.nodes.at(node_index).rotation, XMQuaternionSlerp(R[0], R[1], factor));
		
		XMVECTOR T[2]{ XMLoadFloat3(&keyframes[0]->nodes.at(node_index).translation), XMLoadFloat3(&keyframes[1]->nodes.at(node_index).translation) };
		XMStoreFloat3(&keyframe.nodes.at(node_index).translation, XMVectorLerp(T[0], T[1], factor));
	}
}

inline XMFLOAT4X4 skinned_mesh::to_xmfloat4x4(const FbxAMatrix& fbxamatrix)
{
	XMFLOAT4X4 xmfloat4x4;
	for (int row = 0; row < 4; ++row)
	{
		for (int column = 0; column < 4; ++column)
		{
			xmfloat4x4.m[row][column] = static_cast<float>(fbxamatrix[row][column]);
		}
	}
	return xmfloat4x4;
}

inline XMFLOAT3 skinned_mesh::to_xmfloat3(const FbxDouble3 & fbxdouble3)
{
	XMFLOAT3 xmfloat3;
	xmfloat3.x = static_cast<float>(fbxdouble3[0]);
	xmfloat3.y = static_cast<float>(fbxdouble3[1]);
	xmfloat3.z = static_cast<float>(fbxdouble3[2]);
	return xmfloat3;
}

inline XMFLOAT4 skinned_mesh::to_xmfloat4(const FbxDouble4 & fbxdouble4)
{
	XMFLOAT4 xmfloat4;
	xmfloat4.x = static_cast<float>(fbxdouble4[0]);
	xmfloat4.y = static_cast<float>(fbxdouble4[1]);
	xmfloat4.z = static_cast<float>(fbxdouble4[2]);
	xmfloat4.w = static_cast<float>(fbxdouble4[3]);
	return xmfloat4;
}

void skinned_mesh::create_com_objects(ID3D11Device* device, const char* fbx_filename)
{
	for (mesh& mesh : meshes)
	{
		HRESULT hr{ S_OK };

		D3D11_BUFFER_DESC buffer_desc{};
		D3D11_SUBRESOURCE_DATA subresource_data{};

		buffer_desc.ByteWidth = static_cast<UINT>(sizeof(vertex) * mesh.vertices.size());
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;

		subresource_data.pSysMem = mesh.vertices.data();
		subresource_data.SysMemPitch = 0;
		subresource_data.SysMemSlicePitch = 0;

		hr = device->CreateBuffer(&buffer_desc, &subresource_data,mesh.vertex_buffer.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

#ifdef _DEBUG
		XMFLOAT3 bbox_vertices[8] =
		{
			{mesh.bounding_box[0].x, mesh.bounding_box[0].y, mesh.bounding_box[0].z},
			{mesh.bounding_box[0].x, mesh.bounding_box[0].y, mesh.bounding_box[1].z},
			{mesh.bounding_box[0].x, mesh.bounding_box[1].y, mesh.bounding_box[0].z},
			{mesh.bounding_box[0].x, mesh.bounding_box[1].y, mesh.bounding_box[1].z},
			{mesh.bounding_box[1].x, mesh.bounding_box[0].y, mesh.bounding_box[0].z},
			{mesh.bounding_box[1].x, mesh.bounding_box[0].y, mesh.bounding_box[1].z},
			{mesh.bounding_box[1].x, mesh.bounding_box[1].y, mesh.bounding_box[0].z},
			{mesh.bounding_box[1].x, mesh.bounding_box[1].y, mesh.bounding_box[1].z},
		};

		WORD bbox_indices[24] =
		{
			0,1, 1,3, 3,2, 2,0, // Back face
			4,5, 5,7, 7,6, 6,4, // Front face
			0,4, 1,5, 2,6, 3,7, // Side faces
		};

		buffer_desc.ByteWidth = sizeof(bbox_vertices);

		subresource_data.pSysMem = bbox_vertices;
		
		hr = device->CreateBuffer(&buffer_desc, &subresource_data, mesh.bbox_vertex_buffer.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		buffer_desc.ByteWidth = sizeof(bbox_indices);
		buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		subresource_data.pSysMem = bbox_indices;

		hr = device->CreateBuffer(&buffer_desc, &subresource_data, mesh.bbox_index_buffer.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
#endif // DEBUG


		buffer_desc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * mesh.indices.size());
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		
		subresource_data.pSysMem = mesh.indices.data();
		hr = device->CreateBuffer(&buffer_desc, &subresource_data,mesh.index_buffer.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
#if 1
		mesh.vertices.clear();
		mesh.indices.clear();
#endif
	}

	HRESULT hr = S_OK;
	D3D11_INPUT_ELEMENT_DESC input_element_desc[]
	{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA , 0},
		{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA , 0},
		{ "TANGENT",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA , 0},
		{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA , 0},
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA , 0},
		{ "WEIGHTS",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT },
		{ "BONES",		0, DXGI_FORMAT_R32G32B32A32_UINT,	0, D3D11_APPEND_ALIGNED_ELEMENT },
	};

	create_vs_from_cso(device, "skinned_mesh_vs.cso", vertex_shader.ReleaseAndGetAddressOf(),
		input_layout.ReleaseAndGetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
	create_ps_from_cso(device, "skinned_mesh_ps.cso", pixel_shader.ReleaseAndGetAddressOf());

#ifdef _DEBUG
	D3D11_INPUT_ELEMENT_DESC b_box_input_element_desc[]
	{
		{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	create_vs_from_cso(device, "bbox_vs.cso", bbox_vertex_shader.GetAddressOf(),
		bbox_input_layout.GetAddressOf(), b_box_input_element_desc, _countof(b_box_input_element_desc));
	create_ps_from_cso(device, "bbox_ps.cso", bbox_pixel_shader.GetAddressOf());
#endif // _DEBUG


	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.ByteWidth = sizeof(constants);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffer.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	for (std::unordered_map<uint64_t, material>::iterator iterator = materials.begin(); iterator != materials.end(); ++iterator)
	{
		for (size_t texture_index = 0; texture_index < 2; ++texture_index)
		{
			if (iterator->second.texture_filenames[texture_index].size() > 0)
			{
				std::filesystem::path path(fbx_filename);
				path.replace_filename(iterator->second.texture_filenames[texture_index]);
				D3D11_TEXTURE2D_DESC texture2d_desc;
				load_texture_from_file(device, path.c_str(), iterator->second.shader_resource_views[texture_index].GetAddressOf(), &texture2d_desc);
			}
			else
			{
				make_dummy_texture(device, iterator->second.shader_resource_views[texture_index].GetAddressOf(), texture_index == 1 ? 0xFFFF7F7F : 0xFFFFFFFF, 16);
			}
		}
	}
}
