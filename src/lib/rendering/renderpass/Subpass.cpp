/* SUBPASS.cpp
 *   by Lut99
 *
 * Created:
 *   29/06/2021, 13:30:04
 * Last edited:
 *   29/06/2021, 13:30:04
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Subpass class, which is used internally by the RenderPass
 *   to manage a subpass' memory.
**/

#include "tools/CppDebugger.hpp"
#include "../auxillary/ErrorCodes.hpp"

#include "Subpass.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Rendering;
using namespace CppDebugger::SeverityValues;


/***** POPULATE FUNCTIONS *****/
/* Populates the given VkSubpassDescription struct. */
static void populate_subpass(VkSubpassDescription& subpass, VkAttachmentReference* color_attachment_refs, uint32_t n_attachments, VkAttachmentReference* depth_attachment_ref, VkPipelineBindPoint bind_point) {
    DENTER("populate_subpass");

    // Set to default
    subpass = {};

    // Set the bindpoint
    subpass.pipelineBindPoint = bind_point;

    // Set the color attachments
    subpass.colorAttachmentCount = n_attachments;
    subpass.pColorAttachments = color_attachment_refs;

    // Set the depth attachment
    subpass.pDepthStencilAttachment = depth_attachment_ref;

    // In the future, more types of attachments may be given here
    subpass.inputAttachmentCount = 0;
    subpass.preserveAttachmentCount = 0;

    // Done
    DRETURN;
}





/***** SUBPASS CLASS *****/
/* Constructor for the Subpass class, which takes a list of attachment index/layout pairs to describe the colour references, an index/layout pair for the depth reference and the bind point in the pipeline (which is usually the default of graphics). */
Subpass::Subpass(const Tools::Array<std::pair<uint32_t, VkImageLayout>>& color_attachment_refs, const std::pair<uint32_t, VkImageLayout>& depth_attachment_ref, VkPipelineBindPoint bind_point) :
    color_attachment_refs(new VkAttachmentReference[color_attachment_refs.size()]),
    n_color_attachments(color_attachment_refs.size()),
    depth_attachment_ref(new VkAttachmentReference)
{
    DENTER("Rendering::Subpass::Subpass");

    // Populate the attachment reference list
    for (uint32_t i = 0; i < this->n_color_attachments; i++) {
        this->color_attachment_refs[i] = {};
        this->color_attachment_refs[i].attachment = color_attachment_refs[i].first;
        this->color_attachment_refs[i].layout = color_attachment_refs[i].second;
    }

    // Populate the depth attachment
    *this->depth_attachment_ref = {};
    this->depth_attachment_ref->attachment = depth_attachment_ref.first;
    this->depth_attachment_ref->layout = depth_attachment_ref.second;

    // With that and the bind point, populate the VkSubpassDescription struct
    populate_subpass(this->vk_subpass, this->color_attachment_refs, this->n_color_attachments, this->depth_attachment_ref, bind_point);

    DLEAVE;
}

/* Copy constructor for the Subpass class. */
Subpass::Subpass(const Subpass& other) :
    vk_subpass(other.vk_subpass),
    color_attachment_refs(new VkAttachmentReference[other.n_color_attachments]),
    n_color_attachments(other.n_color_attachments),
    depth_attachment_ref(new VkAttachmentReference(*other.depth_attachment_ref))
{
    DENTER("Rendering::Subpass::Subpass(copy)");

    // Copy the elements over from the old to the new list of attachment references
    memcpy(this->color_attachment_refs, other.color_attachment_refs, this->n_color_attachments * sizeof(VkAttachmentReference));

    // Mark the new array as the one in the internal list
    this->vk_subpass.pColorAttachments = this->color_attachment_refs;
    this->vk_subpass.pDepthStencilAttachment = this->depth_attachment_ref;

    DLEAVE;
}

/* Move constructor for the Subpass class. */
Subpass::Subpass(Subpass&& other) :
    vk_subpass(other.vk_subpass),
    color_attachment_refs(other.color_attachment_refs),
    n_color_attachments(other.n_color_attachments),
    depth_attachment_ref(other.depth_attachment_ref)
{
    // Make sure the list is not deallocated
    other.color_attachment_refs = nullptr;
    other.depth_attachment_ref = nullptr;
}

/* Destructor for the Subpass class. */
Subpass::~Subpass() {
    DENTER("Rendering::Subpass::~Subpass");

    // If needed, deallocate the attachment references
    if (this->depth_attachment_ref != nullptr) {
        delete this->depth_attachment_ref;
    }
    if (this->color_attachment_refs != nullptr) {
        delete[] this->color_attachment_refs;
    }

    DLEAVE;
}



/* Swap operator for the Subpass class. */
void Rendering::swap(Subpass& s1, Subpass& s2) {
    using std::swap;

    swap(s1.vk_subpass, s2.vk_subpass);
    swap(s1.color_attachment_refs, s2.color_attachment_refs);
    swap(s1.n_color_attachments, s2.n_color_attachments);
    swap(s1.depth_attachment_ref, s2.depth_attachment_ref);
}
