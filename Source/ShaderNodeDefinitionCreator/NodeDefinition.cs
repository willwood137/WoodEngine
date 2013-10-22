using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ShaderNodeDefinitionCreator
{

    enum SHADER_KEYWORD
    {
        SHADER_KEYWORD_GL,
        SHADER_KEYWORD_VARYING,
        SHADER_KEYWORD_OUT,
        SHADER_KEYWORD_SIZE
    }

    struct KeyWord
    {
        SHADER_KEYWORD type;
        string name;
    }

    enum TYPE_VALUE
    {
        TYPE_VALUE_FLOAT,
        TYPE_VALUE_MATRIX,
        TYPE_VALUE_SIZE
    }

    struct DataType
    {
        TYPE_VALUE type;
        int minSize;
        int maxSize;
    }

    struct NodeLink
    {
        string name;
        bool canBeVert;
        bool canBeFrag;
        bool OutLink;
        DataType dType;
        List<KeyWord> keywords;
    }

    struct Parameter
    {
        TYPE_VALUE type;
        int size;
        string name;
    }

    struct FunctionDefinition
    {
        List<Parameter> parameters;
        TYPE_VALUE returnType;
        int returnSize;
    }

    class NodeDefinition
    {
        public string name;
        public string category;
        public bool canBeVert;
        public bool canBeFrag;
        public List<NodeLink> inputs;
        public List<NodeLink> outputs;
        public List<FunctionDefinition> functions;

    }

    struct NodeCategory
    {
        string name;
        List<NodeDefinition> nodes;
    }
}
