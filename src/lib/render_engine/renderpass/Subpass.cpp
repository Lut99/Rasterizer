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
#include "render_engine/auxillary/ErrorCodes.hpp"

#include "Subpass.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Rendering;
using namespace CppDebugger::SeverityValues;


/***** POPULATE FUNCTIONS *****/
/* Populates the given VkSubpassDescription struct. */
static void populate_subpass(VkSubpassDescription& subpass, VkAttachmentReference* attachment_refs, uint32_t n_attachments, VkPipelineBindPoint bind_point) {
    DENTER("populate_subpass");

    // Set to default
    subpass = {};

    // Set the bindpoint
    subpass.pipelineBindPoint = bind_point;

    // Set the color attachments
    subpass.colorAttachmentCount = n_attachments;
    subpass.pColorAttachments = attachment_refs;

    // In the future, more types of attachments may be given here
    subpass.inputAttachmentCount = 0;
    subpass.preserveAttachmentCount = 0;

    // Done
    DRETURN;
}





/***** SUBPASS CLASS *****/
/* Constructor for the Subpass class, which takes a list of attachment index/layout pairs to describe the references, and the bind point in the pipeline (which is usually the default of graphics). */
Subpass::Subpass(const Tools::Array<std::pair<uint32_t, VkImageLayout>>& attachment_refs, VkPipelineBindPoint bind_point) :
    attachment_refs(new VkAttachmentReference[attachment_refs.size()]),
    n_attachments(attachment_refs.size())
{
    DENTER("Rendering::Subpass::Subpass");

    // Populate the attachment reference list
    for (uint32_t i = 0; i < this->n_attachments; i++) {
        this->attachment_refs[i] = {};
        this->attachment_refs[i].attachment = attachment_refs[i].first;
        this->attachment_refs[i].layout = attachment_refs[i].second;
    }

    // With that and the bind point, populate the VkSubpassDescription struct
    populate_subpass(this->vk_subpass, this->attachment_refs, this->n_attachments, bind_point);

    DLEAVE;
}

/* Copy constructor for the Subpass class. */
Subpass::Subpass(const Subpass& other) :
    vk_subpass(other.vk_subpass),
    attachment_refs(new VkAttachmentReference[other.n_attachments]),
    n_attachments(other.n_attachments)
{
    DENTER("Rendering::Subpass::Subpass(copy)");

    // Copy the elements over from the old to the new list of attachment references
    memcpy(this->attachment_refs, other.attachment_refs, this->n_attachments * sizeof(VkAttachmentReference));

    // Mark the new array as the one in the internal list
    this->vk_subpass.pColorAttachments = this->attachment_refs;

    DLEAVE;
}

/* Move constructor for the Subpass class. */
Subpass::Subpass(Subpass&& other) :
    vk_subpass(other.vk_subpass),
    attachment_refs(other.attachment_refs),
    n_attachments(other.n_attachments)
{
    // Make sure the list is not deallocated
    other.attachment_refs = nullptr;
}

/* Destructor for the Subpass class. */
Subpass::~Subpass() {
    DENTER("Rendering::Subpass::~Subpass");

    // If needed, deallocate the attachment references
    if (this->attachment_refs != nullptr) {
        delete[] this->attachment_refs;
    }

    DLEAVE;
}



/* Swap operator for the Subpass class. */
void Rendering::swap(Subpass& s1, Subpass& s2) {
    using std::swap;

    swap(s1.vk_subpass, s2.vk_subpass);
    swap(s1.attachment_refs, s2.attachment_refs);
    swap(s1.n_attachments, s2.n_attachments);
}
