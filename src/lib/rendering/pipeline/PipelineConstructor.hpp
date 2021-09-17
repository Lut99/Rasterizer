/* PIPELINE CONSTRUCTOR.hpp
 *   by Lut99
 *
 * Created:
 *   17/09/2021, 21:43:24
 * Last edited:
 *   17/09/2021, 21:43:24
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the PipelineConstructor class, which can be used to define
 *   how a Pipeline needs to be created.
**/

#ifndef RENDERING_PIPELINE_CONSTRUCTOR_HPP
#define RENDERING_PIPELINE_CONSTRUCTOR_HPP

#include "tools/Array.hpp"

#include "properties/ShaderStage.hpp"

namespace Makma3D::Rendering {
    /* The PipelineConstructor class, which is used to efficiently set a pipeline's properties. */
    class PipelineConstructor {
    public:
        /* Channel name for the PipelineCOnstructor class. */
        static constexpr const char* channel = "PipelineConstructor";

    public:
        /* List of Shaders to load,each with their matching pipeline stage and map of specialization constants. */
        Tools::Array<ShaderStage> shaders;

    public:
        /* Default constructor for the PipelineConstructor class. */
        PipelineConstructor();
        /* Copy constructor for the PipelineConstructor class. */
        PipelineConstructor(const PipelineConstructor& other);
        /* Move constructor for the PipelineConstructor class. */
        PipelineConstructor(PipelineConstructor&& other);
        /* Destructor for the PipelineConstructor class. */
        ~PipelineConstructor();

        /* Adds a new shader to the PipelineConstructor, bound at the given stage and with the given specialization constants. */
        void add_shader(const Rendering::Shader* shader, VkShaderStageFlags vk_shader_stage, const std::unordered_map<uint32_t, std::pair<void*, uint32_t>>& specialization_constants);

        /* Copy assignment operator for the PipelineConstructor class. */
        inline PipelineConstructor& operator=(const PipelineConstructor& other) { return *this = PipelineConstructor(other); }
        /* Move assignment operator for the PipelineConstructor class. */
        inline PipelineConstructor& operator=(PipelineConstructor&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the PipelineConstructor class. */
        friend void swap(PipelineConstructor& pc1, PipelineConstructor& pv2);

    };
    
    /* Swap operator for the PipelineConstructor class. */
    void swap(PipelineConstructor& pc1, PipelineConstructor& pv2);

}

#endif
