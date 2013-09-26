
#include <string>
#include <vector>

#include "Model.hpp"

namespace woodman
{

	struct Attribute
	{
		std::string name;
		unsigned int numValues;
		unsigned int id;
	};

	enum DataType
	{
		DATATYPE_FLOAT,
		DATATYPE_MATRIX,
	};

	struct Uniform
	{
		std::string name;
		DataType dType;
		unsigned int numValues;
		unsigned int id;
	};

	class MetaShader
	{
	public:

		MetaShader(const std::string& shaderName);

		bool LoadShader();
		

		void render( Model* modelToRender );
	private:
		
		std::vector<Attribute> m_attributes;
		std::vector<Uniform> m_uniforms;
		std::string m_shaderName;
		unsigned int m_shaderID;
	}; 
}
