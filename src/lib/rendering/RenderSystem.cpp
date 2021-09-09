/* RENDER SYSTEM.cpp
 *   by Lut99
 *
 * Created:
 *   20/07/2021, 15:10:25
 * Last edited:
 *   07/08/2021, 15:23:48
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Defines the rendering system, which is capable of showing the entities
 *   that have a Render component on the screen. Also uses the Transform
 *   component to decide where to place the entity.
**/

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ecs/components/Transform.hpp"
#include "ecs/components/Meshes.hpp"
#include "ecs/components/Camera.hpp"
#include "ecs/components/Textures.hpp"
#include "models/ModelSystem.hpp"

#include "auxillary/ErrorCodes.hpp"
#include "auxillary/Rectangle.hpp"
#include "auxillary/Vertex.hpp"
#include "auxillary/Index.hpp"

#include "RenderSystem.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::ECS;
using namespace Rasterizer::Rendering;


/***** RENDERSYSTEM CLASS *****/
/* Constructor for the RenderSystem, which takes a window, a memory manager to render (to and draw memory from, respectively), a model system to schedule the model buffers with and a texture system to schedule texture images with. */
RenderSystem::RenderSystem(Window& window, MemoryManager& memory_manager, const Models::ModelSystem& model_system, const Textures::TextureSystem& texture_system) :
    window(window),
    memory_manager(memory_manager),
    model_system(model_system),
    texture_system(texture_system),

    global_descriptor_layout(this->window.gpu()),
    object_descriptor_layout(this->window.gpu()),

    depth_stencil(this->window.gpu(), this->memory_manager.draw_pool, this->window.swapchain().extent()),

    vertex_shader(this->window.gpu(), "bin/shaders/vertex_v5.spv"),
    fragment_shader(this->window.gpu(), "bin/shaders/frag_v1.spv"),
    render_pass(this->window.gpu()),
    pipeline(this->window.gpu())
{

    // Initialize the descriptor set layout for the global data
    this->global_descriptor_layout.add_binding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);
    this->global_descriptor_layout.finalize();

    // Initialize the descriptor set layout for the per-object data
    this->object_descriptor_layout.add_binding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);
    this->object_descriptor_layout.finalize();

    // Initialize the render pass
    uint32_t col_index = this->render_pass.add_attachment(this->window.swapchain().format(), VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    uint32_t dep_index = this->render_pass.add_attachment(this->depth_stencil.format(), VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    this->render_pass.add_subpass({ std::make_pair(col_index, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) }, std::make_pair(dep_index, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL));
    this->render_pass.add_dependency(VK_SUBPASS_EXTERNAL, col_index, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, 0, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);
    this->render_pass.finalize();

    // Prepare the pipeline by choosing all its settings
    this->pipeline.init_shader_stage(this->vertex_shader, VK_SHADER_STAGE_VERTEX_BIT);
    this->pipeline.init_shader_stage(this->fragment_shader, VK_SHADER_STAGE_FRAGMENT_BIT);
    this->pipeline.init_input_assembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    this->pipeline.init_depth_testing(VK_TRUE, VK_COMPARE_OP_LESS);
    this->pipeline.init_viewport_transformation(Rectangle(0.0, 0.0, this->window.swapchain().extent()), Rectangle(0.0, 0.0, this->window.swapchain().extent()));
    this->pipeline.init_rasterizer(VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE); //VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE);
    this->pipeline.init_multisampling();
    this->pipeline.init_color_blending(0, VK_FALSE, VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD, VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD);
    this->pipeline.init_color_logic(VK_FALSE, VK_LOGIC_OP_NO_OP);
    this->pipeline.init_pipeline_layout({ this->global_descriptor_layout, this->object_descriptor_layout }, {});
    this->pipeline.finalize(this->render_pass, 0);

    // Initialize the frame manager
    this->frame_manager = new FrameManager(this->memory_manager, this->window.swapchain(), this->global_descriptor_layout, this->object_descriptor_layout);
    this->frame_manager->bind(this->render_pass, this->depth_stencil);

    // Done initializing
    logger.logc(Verbosity::important, RenderSystem::channel, "Init success.");
}

/* Move constructor for the RenderSystem class. */
RenderSystem::RenderSystem(RenderSystem&& other)  :
    window(other.window),
    memory_manager(other.memory_manager),
    model_system(other.model_system),
    texture_system(other.texture_system),

    global_descriptor_layout(other.global_descriptor_layout),
    object_descriptor_layout(other.object_descriptor_layout),

    depth_stencil(other.depth_stencil),

    vertex_shader(other.vertex_shader),
    fragment_shader(other.fragment_shader),
    render_pass(other.render_pass),
    pipeline(other.pipeline),

    frame_manager(other.frame_manager)
{
    // Prevent the frame manager from being deallocated
    this->frame_manager = nullptr;
}

/* Destructor for the RenderSystem class. */
RenderSystem::~RenderSystem() {
    logger.logc(Verbosity::important, RenderSystem::channel, "Cleaning...");

    // Deallocate the frame manager if needed
    if (this->frame_manager != nullptr) {
        delete this->frame_manager;
    }

    logger.logc(Verbosity::important, RenderSystem::channel, "Cleaned.");
}



/* Private helper function that resizes all required structures for a new window size. */
void RenderSystem::_resize() {
    // First, wait until the device is idle
    this->window.gpu().wait_for_idle();

    // Then, resize the window
    this->window.resize();

    // Re-create the depth stencil with a new size
    logger.debug("Window size: ", this->window.real_extent().width, 'x', this->window.real_extent().height);
    this->depth_stencil.resize(this->window.real_extent());

    // Re-create all frames in the frame manager
    this->frame_manager->bind(this->render_pass, this->depth_stencil);
}



/* Runs a single iteration of the game loop. Returns whether or not the RenderSystem is asked to close the window (false) or not (true). */
bool RenderSystem::render_frame(const ECS::EntityManager& entity_manager) {
    /* PREPARATION */
    // First, handle window events
    bool can_continue = this->window.loop();
    if (!can_continue) {
        return false;
    }

    // Next, get a conceptual frame to render to
    ConceptualFrame* frame = this->frame_manager->get_frame();
    if (frame == nullptr) {
        // Resize, then stop this iteration
        this->_resize();
        return true;
    }

    // Prepare rendering to the frame
    const ECS::ComponentList<ECS::Meshes>& objects = entity_manager.get_list<ECS::Meshes>();
    frame->prepare_render(objects.size());

    // Populate the frame's camera data
    const Camera& cam = entity_manager.get_list<Camera>()[0];
    frame->upload_camera_data(cam.proj, cam.view);





    /* RECORDING */
    // Start recording the frame's command buffer
    frame->schedule_start(this->pipeline);

    // Schedule the global data too
    frame->schedule_global();

    // Next, loop through all objects
    for (uint32_t i = 0; i < objects.size(); i++) {
        // Get the relevant components for this entity
        entity_t entity = objects.get_entity(i);
        const ECS::Meshes& meshes = objects[i];
        const ECS::Transform& transform = entity_manager.get_component<Transform>(entity);

        // Populate the buffer for this entity
        frame->upload_object_data(i, ObjectData{ transform.translation });
        // Schedule the object's buffer too
        frame->schedule_object(i);

        // Loop through all meshes of this object to render them
        for (uint32_t j = 0; j < meshes.size(); j++) {
            // Schedule its draw
            frame->schedule_draw(this->model_system, meshes[j]);
        }
    }

    // Close off recording
    frame->schedule_stop();





    /* PRESENTING */
    // 'Render' the frame by submitting it
    VkQueue graphics_queue = this->window.gpu().queues(QueueType::graphics)[0];
    frame->submit(graphics_queue);

    // Schedule the frame for presentation once it's done rendering
    if (this->window.needs_resize() || this->frame_manager->present_frame(frame)) {
        // Resize the window
        this->_resize();
    }





    /* RETURNING */
    return true;
}



/* Swap operator for the RenderSystem class. */
void Rendering::swap(RenderSystem& rs1, RenderSystem& rs2) {
    #ifndef NDEBUG
    if (&rs1.window != &rs2.window) { logger.fatalc(RenderSystem::channel, "Cannot swap render systems with different windows"); }
    if (&rs1.memory_manager != &rs2.memory_manager) { logger.fatalc(RenderSystem::channel, "Cannot swap render systems with different memory managers"); }
    if (&rs1.model_system != &rs2.model_system) { logger.fatalc(RenderSystem::channel, "Cannot swap render systems with different model systems"); }
    if (&rs1.texture_system != &rs2.texture_system) { logger.fatalc(RenderSystem::channel, "Cannot swap render systems with different texture systems"); }
    #endif

    // Simply swap everything
    using std::swap;

    swap(rs1.global_descriptor_layout, rs2.global_descriptor_layout);
    swap(rs1.object_descriptor_layout, rs2.object_descriptor_layout);

    swap(rs1.depth_stencil, rs2.depth_stencil);

    swap(rs1.vertex_shader, rs2.vertex_shader);
    swap(rs1.fragment_shader, rs2.fragment_shader);
    swap(rs1.render_pass, rs2.render_pass);
    swap(rs1.pipeline, rs2.pipeline);

    swap(rs1.frame_manager, rs2.frame_manager);
}
