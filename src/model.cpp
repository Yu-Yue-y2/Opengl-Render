#include "model.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "macro.h"
#define PUSH_VEC(VECS,VEC) {\
for(int i = 0;i<(VEC).length();i++)\
    VECS.push_back(VEC[i]);}
#define PUSH_CONS(VECS, CONST, LENTH) {\
for(int i = 0;i<LENTH;i++)\
    VECS.push_back(CONST);}
#define COUT_VEC(VEC)\
for(int i = 0;i<(VEC).length();i++)\
    std::cout<<(VEC[i])<<" ";


Model::Model(const char* path, bool caltbn)
{
    std::ifstream in;
    in.open(path, std::ifstream::in);
    if (in.fail()) 
    {
        std::cout << "can't open" << path << std::endl;
        return;
    }

    std::string line;
    std::vector<glm::vec3> poses;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    std::vector<glm::ivec3> ids;
    while (!in.eof())
    {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v "))
        {
            iss >> trash;
            glm::vec3 v;
            for (int i = 0; i < 3; i++)
                iss >> v[i];
            poses.push_back(v);
        }
        else if (!line.compare(0, 2, "f "))
        {
            int idx, udx, ndx;
            std::vector<glm::ivec3> temp_ids;
            iss >> trash;
            while (iss >> idx) {
                iss.get();
                if (iss.peek() == '/') {
                    iss.get(); // 跳过第一个 '/'
                    iss >> ndx;
                    glm::ivec3 id(idx - 1, ndx - 1, -1);
                    temp_ids.push_back(id);
                }
                else {
                    iss >> udx;
                    iss.get(); // 跳过 '/'
                    iss >> ndx;
                    glm::ivec3 id(idx - 1, ndx - 1, udx - 1);
                    temp_ids.push_back(id);
                }
            }
            if (temp_ids.size() == 3)
            {
                for (auto id : temp_ids)
                    ids.push_back(id);
            }
            else if (temp_ids.size() == 4)
            {
                static int index[] = { 1,2,3,1,3,4 };
                for (int i = 0; i < 6; i++)
                {
                    ids.push_back(temp_ids[index[i] - 1]);
                }
            }
        }
        else if (!line.compare(0, 3, "vt "))
        {
            iss >> trash >> trash;
            glm::vec2 _uv;
            for (int i = 0; i < 2; i++)
                iss >> _uv[i];
            uvs.push_back(_uv);
        }
        else if (!line.compare(0, 3, "vn "))
        {
            iss >> trash >> trash;
            glm::vec3 normal;
            for (int i = 0; i < 3; i++)
                iss >> normal[i];
            normals.push_back(normal);
        }
    }
    std::vector<glm::vec3> ts, bs;
    if (caltbn)
    {
        for (int i = 0;i + 2 < ids.size(); i += 3)
        {
            glm::vec3 pos1 = poses[ids[i][0]];
            glm::vec2 uv1 = uvs[ids[i][2]];
            glm::vec2 uv2 = uvs[ids[i + 1][2]];
            glm::vec3 pos2 = poses[ids[i + 1][0]];
            glm::vec2 uv3 = uvs[ids[i + 2][2]];
            glm::vec3 pos3 = poses[ids[i + 2][0]];
            glm::vec3 e1 = pos2 - pos1;
            glm::vec3 e2 = pos3 - pos2;
            glm::vec2 uv12 = uv2 - uv1;
            glm::vec2 uv23 = uv3 - uv2;
            
            float f = 1.0 / (uv12[0] * uv23[1] - uv12[1] * uv23[0]);
            float a00 = uv23[1], a01 = -uv12[1], a10 = -uv23[0], a11 = uv12[0];
            glm::vec3 t, b;
            t[0] = a00 * e1.x + a01 * e2.x;
            t[1] = a00 * e1.y + a01 * e2.y;
            t[2] = a00 * e1.z + a01 * e2.z;

            b[0] = a10 * e1.x + a11 * e2.x;
            b[1] = a10 * e1.y + a11 * e2.y;
            b[2] = a10 * e1.z + a11 * e2.z;
            t = f * t;
            b = f * b;
            ts.push_back(t);
            bs.push_back(b);
        }
    }
    if (uvs.size() > 0)
        this->has_uv = true;
    if (caltbn) 
    {
        if (!has_uv)
        {
            std::cerr<< "error in uv and tbn of model loading" << std::endl;
            return;
        }
        if (ts.size() == 0 || bs.size() == 0)
        {
            std::cerr << "error in tbn of model loading" << std::endl;
            return;
        }
        this->has_tbn = true;
    }
    if (normals.size() > 0)
    {
        has_normal = true;
    }

    int count = 0;
    for (auto id : ids)
    {
        PUSH_VEC(data, poses[id[0]]);
        if (has_normal)
            PUSH_VEC(data, normals[id[1]]);
        if (id[2] != -1) {
            PUSH_VEC(data, uvs[id[2]]);
            if (has_tbn) {
                PUSH_VEC(data, ts[count / 3]);
                PUSH_VEC(data, bs[count / 3]);
                 count++;
            }
        }
        else 
        {
            PUSH_CONS(data, 0.0, 2);
            if (has_tbn)
            {
                std::cerr << "un valid uv information to calculate tbn" << std::endl;
                return;
            }
        }
    }
    
}
void Model::DirectlyLoadData(std::vector<float>& _data)
{
    data = _data;
}
uPtr<VerData> Model::LoadData()
{
    int size = 3;
    if (has_normal)
        size += 3;
    if (has_uv)
        size += 2;
    if (has_tbn)
        size += 6;
    uPtr<VerData> vbo = mkU<VerData>();
    vbo->LoadDate(data, data.size() / size, size);
    return vbo;
}
void Model::BindVaoData()
{
    vao->Active();
    if (vao->ebo)
    {
        vao->ebo->Bind();
        vao->ebo->DisActive();
    }
    int count = 0;
    for (const uPtr<VerData>& vbo : vao->vbos)
    {
        //TODO IN FUTURE
        vao->tri_n += vbo->tri_n;
        vbo->Bind();
        vbo->Analyse(LAYOUT_(count++), 3, GL_FLOAT, GL_FALSE, vbo->ver_size * sizeof(float), (void*)0);
        int offset = 3;
        if (has_normal)
        {
            vbo->Analyse(LAYOUT_(count++), 3, GL_FLOAT, GL_FALSE, vbo->ver_size * sizeof(float), (void*)(offset * sizeof(float)));
            offset += 3;
        }
        if (has_uv)
        {
            vbo->Analyse(LAYOUT_(count++), 2, GL_FLOAT, GL_FALSE, vbo->ver_size * sizeof(float), (void*)(offset * sizeof(float)));
            offset += 2;
        }
        if (has_tbn)
        {
            vbo->Analyse(LAYOUT_(count++), 3, GL_FLOAT, GL_FALSE, vbo->ver_size * sizeof(float), (void*)(offset * sizeof(float)));
            offset += 3;
            vbo->Analyse(LAYOUT_(count++), 3, GL_FLOAT, GL_FALSE, vbo->ver_size * sizeof(float), (void*)(offset * sizeof(float)));
            offset += 3;
        }
        if (offset != vbo->ver_size)
        {
            std::cout << "error in vao bind data.vbo vertex size is not campatible with calculated results in vao" << std::endl;
        }
        vbo->DisBind();
        std::cout << offset << " normal: " << has_normal << " uv: " << has_uv << " tbn: " << has_tbn << std::endl;
    }
    vao->DisActive();
}
void Model::ClearData()
{
    std::vector<float> _data;
    std::swap(_data, data);
}
void Model::SetVaoData()
{
    uPtr<VerData> vbo = LoadData();
    vao = mkU<VertexArray>();
    vao->vbos.push_back(std::move(vbo));
    BindVaoData();
}
void Model::Render() const
{
    vao->Active();
    if (vao->ebo) {
        vao->ebo->Active();
        glDrawElements(GL_TRIANGLES, vao->ebo->ind_n, GL_UNSIGNED_INT, 0);
    }
    else
    {
        glDrawArrays(GL_TRIANGLES, 0, vao->tri_n * 3);
    }
    vao->DisActive();
}
Instance::Instance(Model& _model, glm::mat4 _model_trans, const char* _name, Material mat, ModelType modelt)
    :model(_model), model_transform(_model_trans), normal_transform(glm::transpose(glm::inverse(_model_trans))), 
    modelType(modelt), material(mat), name(_name)
{}

ModelType Instance::GetModelType() const
{
    return modelType;
}
const Model& Instance::GetModel() const
{
    return model;
}
glm::mat4 Instance::GetModelMatrix() const
{
    return model_transform;
}
glm::mat4 Instance::GetNormalMatrix() const
{
    return normal_transform;
}
void Instance::SetMaterial(Material _material)
{
    material = _material;
}
Material Instance::GetMaterial() const
{
    return material;
}
void Instance::SetFollowLight(const Light* light)
{
    followLight = light;
}
const Light* Instance::GetLigtht() const
{
    return followLight;
}
glm::vec3 Instance::GetPosition() const
{
    return glm::vec3(model_transform[3][0], model_transform[3][1], model_transform[3][2]);
}
void Instance::SetPosition(glm::vec3 pos)
{
    for (int i = 0; i < 3; i++)
        model_transform[3][i] = pos[i];
    normal_transform = glm::transpose(glm::inverse(model_transform));
}
void Instance::SetModelTransform(glm::mat4 m)
{
    model_transform = m;
}