#pragma once
#include "game_object.h"
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include <fbxsdk.h>
#include <unordered_map>

struct scene
{
	struct node
	{
		uint64_t unique_id{ 0 };
		std::string name;
		FbxNodeAttribute::EType attribute{ FbxNodeAttribute::EType::eUnknown };
		int64_t parent_index{ -1 };

		template < class T>
		void serialize(T & archive)
		{
			archive(unique_id, name, attribute, parent_index);
		}
	};
	std::vector<node> nodes;
	int64_t indexof(uint64_t unique_id) const
	{
		int64_t index{ 0 };
		for (const node& node : nodes)
		{
			if (node.unique_id == unique_id)
			{
				return index;
			}
			++index;
		}
		return -1;
	}

	template < class T>
	void serialize(T & archive)
	{
		archive(nodes);
	}
};

struct skeleton
{
	struct bone
	{
		uint64_t unique_id{ 0 };
		std::string name;
		// 'parent_index' is index that refers to the parent bone's position in the array that contains itself.
		int64_t parent_index{ -1 }; // -1 : the bone is orphan
		// 'node_index' is an index that refers to the node array of the scene.
		int64_t node_index{ 0 };
		
		// 'offset_transform' is used to convert from model(mesh) space to bone(node) scene.
		DirectX::XMFLOAT4X4 offset_transform{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };

		template<class T>
		void serialize(T& archive)
		{
			archive(unique_id, name, parent_index, node_index, offset_transform);
		}
		
		bool is_orphan() const { return parent_index < 0; };
	};
	std::vector<bone> bones;
	int64_t indexof(uint64_t unique_id) const
	{
		int64_t index{ 0 };
		for (const bone& bone : bones)
		{
			if (bone.unique_id == unique_id)
			{
				return index;
			}
			++index;
		}
		return -1;
	}

	template < class T>
	void serialize(T & archive)
	{
		archive(bones);
	}
};

struct animation
{
	std::string name;
	float sampling_rate{ 0 };
	
	struct keyframe
	{
		struct node
		{
			// 'global_transform' is used to convert from local space of node to global space of scene.
			DirectX::XMFLOAT4X4 global_transform{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };

			// The transformation data of a node includes its translation, rotation and scaling vectors with respect to its parent. 
			DirectX::XMFLOAT3 scaling{ 1, 1, 1 };
			DirectX::XMFLOAT4 rotation{ 0, 0, 0, 1 }; // Rotation quaternion
			DirectX::XMFLOAT3 translation{ 0, 0, 0 };

			template < class T>
			void serialize(T & archive)
			{
				archive(global_transform, scaling, rotation, translation);
			}
		};
		std::vector<node> nodes;

		template < class T>
		void serialize(T & archive)
		{
			archive(nodes);
		}
	};
	std::vector<keyframe> sequence;

	template < class T>
	void serialize(T & archive)
	{
		archive(name, sampling_rate, sequence);
	}
};

HRESULT make_dummy_texture(ID3D11Device* device, ID3D11ShaderResourceView** shader_resource_view, DWORD value/*0xAABBGGRR*/, UINT dimension);

class static_mesh : public GAME_OBJECT
{
public:
	// サブセットは対応するマテリアル名、そのマテリアルを使用するメッシュのインデックス開始番号とインデックス数
	struct subset
	{
		std::wstring usemtl;
		uint32_t index_start{ 0 };   // start position of index buffer
		uint32_t index_count{ 0 };   // number of vertices (indices)
	};
	std::vector<subset> subsets;

	// マテリアル情報構造体
	struct material
	{
		std::wstring name;
		DirectX::XMFLOAT4 Ka{ 0.2f, 0.2f, 0.2f, 1.0f };
		DirectX::XMFLOAT4 Kd{ 0.8f, 0.8f, 0.8f, 1.0f };
		DirectX::XMFLOAT4 Ks{ 1.0f, 1.0f, 1.0f, 1.0f };
		std::wstring texture_filenames[2];
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_views[2];

	};
	std::vector<material> materials;

	std::vector<vertex> vertices;
	std::vector<vertex> vertices_world;
	std::vector<uint32_t> indices;
	XMMATRIX world_matrix;

	// バウンディングボックス
	struct bounding_box
	{
		DirectX::XMFLOAT3 min;
		DirectX::XMFLOAT3 max;
	};
	bounding_box b_box;

	static_mesh(ID3D11Device* device, const wchar_t* obj_filename, XMFLOAT3 pos, XMFLOAT3 scl, XMFLOAT3 rot, bool texture_invert);
	~static_mesh();

	void draw(render Render);
	void drawBBox(render Render);
	virtual void update() override;

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader>	bbox_vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	bbox_pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>	bbox_input_layout;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		bbox_vertex_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		bbox_index_buffer;

protected:
	void create_com_buffers(ID3D11Device* device, vertex* vertices, size_t vertex_count,
		uint32_t* indices, size_t index_count);

};

class skinned_mesh : public GAME_OBJECT
{
public:
	static const int MAX_BONE_INFLUENCES{ 4 };
	struct vertex
	{
		XMFLOAT3 position;
		XMFLOAT3 normal;
		XMFLOAT4 tangent;
		XMFLOAT4 color;
		XMFLOAT2 texcoord;
		float bone_weights[MAX_BONE_INFLUENCES]{ 1, 0, 0, 0 };
		uint32_t bone_indices[MAX_BONE_INFLUENCES]{};

		template < class T>
		void serialize(T & archive)
		{
			archive(position, normal, tangent, color, texcoord, bone_weights, bone_indices);
		}
	};

	struct mesh
	{
		uint64_t unique_id{ 0 };
		std::string name;
		// 'node_index' is an index that refers to the node array of the scene.
		int64_t node_index{ 0 };

		struct subset
		{
			uint64_t material_unique_id{ 0 };
			std::string material_name;
			uint32_t start_index_location{ 0 };
			uint32_t index_count{ 0 };

			template < class T>
			void serialize(T & archive)
			{
				archive(material_unique_id, material_name, start_index_location, index_count);
			}
		};
		std::vector<subset> subsets;

		std::vector<vertex> vertices;
		std::vector<uint32_t> indices;

		DirectX::XMFLOAT4X4 default_global_transform{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };

		skeleton bind_pose;

		DirectX::XMFLOAT3 bounding_box[2]
		{
			{ +D3D11_FLOAT32_MAX, +D3D11_FLOAT32_MAX, +D3D11_FLOAT32_MAX },
			{ -D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX }
		};

		template < class T>
		void serialize(T & archive)
		{
			archive(unique_id, name, node_index, subsets, default_global_transform, bind_pose, bounding_box, vertices, indices);
		}

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> bbox_vertex_buffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> bbox_index_buffer;
		friend class skinned_mesh;
	};
	std::vector<mesh> meshes;

	// マテリアル情報構造体
	struct material
	{
		uint64_t unique_id{ 0 };
		std::string name;
		DirectX::XMFLOAT4 Ka{ 0.2f, 0.2f, 0.2f, 1.0f };
		DirectX::XMFLOAT4 Kd{ 0.8f, 0.8f, 0.8f, 1.0f };
		DirectX::XMFLOAT4 Ks{ 1.0f, 1.0f, 1.0f, 1.0f };
		std::string texture_filenames[4];
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_views[4];

		template < class T>
		void serialize(T & archive)
		{
			archive(unique_id, name, Ka, Kd, Ks, texture_filenames);
		}
	};
	std::unordered_map<uint64_t, material> materials;

	enum CoordinateSystem
	{
		RHS_YUP, // 右手系、上方向がY軸
		LHS_YUP, // 左手系、上方向がY軸
		RHS_ZUP, // 右手系、上方向がZ軸
		LHS_ZUP, // 左手系、上方向がZ軸
	};

	enum loadMode
	{
		BINARY,
		ASCII,
	};

	static const int MAX_BONES{ 256 };
	struct constants
	{
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4 material_color;
		DirectX::XMFLOAT4X4 bone_transforms[MAX_BONES]{ { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 } };
	};
	constants constant_data;
	std::vector<animation> animation_clips;
	template < class T>
	void serialize(T & archive)
	{
		archive(animation_clips);
	}


	skinned_mesh(ID3D11Device* device, const char* fbx_filename, bool triangulate , XMFLOAT3 pos, XMFLOAT3 scl, XMFLOAT3 rot, float sampling_rate);
	virtual ~skinned_mesh();

	void fetch_meshes(FbxScene* fbx_scene, std::vector<mesh>& meshes);
	void fetch_materials(FbxScene* fbx_scene, std::unordered_map<uint64_t, material>& materials);
	void fetch_skeleton(FbxMesh* fbx_mesh, skeleton& bind_pose);

	// If sampling_rate is 0, the animation data will be sampled at the default frame rate.
	void fetch_animation(FbxScene* fbx_scene, std::vector<animation>& animation_clips, float sampling_rate);

	void make_dummy_material(std::unordered_map<uint64_t, material>& materials);

	void draw(render Render, XMFLOAT4 material_color, int coordinate_system, const animation::keyframe* keyframe);

	void update_animation(animation::keyframe& keyframe);
	bool append_animations(const char* animation_filename, float sampling_rate, int mode);
	void blend_animations(const animation::keyframe* keyframes[2], float factor, animation::keyframe& keyframe);

private:
	inline XMFLOAT4X4 to_xmfloat4x4(const FbxAMatrix& fbxamatrix);
	inline XMFLOAT3 to_xmfloat3(const FbxDouble3& fbxdouble3);
	inline XMFLOAT4 to_xmfloat4(const FbxDouble4& fbxdouble4);
	Microsoft::WRL::ComPtr<ID3D11VertexShader>	bbox_vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	bbox_pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>	bbox_input_layout;
protected:
	scene scene_view;
	void create_com_objects(ID3D11Device* device, const char* fbx_filename);
};
