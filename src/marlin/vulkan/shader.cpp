//
//  shader.cpp
//  Marlin
//
//  Created by Jonathan Graham on 3/16/23.
//  Copyright © 2023 Jonathan Graham. All rights reserved.
//

#include <marlin/vulkan/shader.hpp>

#include <marlin/vulkan/device.hpp>

#include <fstream>
#include <sstream>

namespace marlin
{

void PrintDescriptorBinding(std::ostream& os, const SpvReflectDescriptorBinding& obj, bool write_set, const char* indent);

uint32_t formatSize( VkFormat format )
{
  uint32_t result = 0;
  switch ( format ) {
    case VK_FORMAT_UNDEFINED:
      result = 0;
      break;
    case VK_FORMAT_R4G4_UNORM_PACK8:
      result = 1;
      break;
    case VK_FORMAT_R4G4B4A4_UNORM_PACK16:
      result = 2;
      break;
    case VK_FORMAT_B4G4R4A4_UNORM_PACK16:
      result = 2;
      break;
    case VK_FORMAT_R5G6B5_UNORM_PACK16:
      result = 2;
      break;
    case VK_FORMAT_B5G6R5_UNORM_PACK16:
      result = 2;
      break;
    case VK_FORMAT_R5G5B5A1_UNORM_PACK16:
      result = 2;
      break;
    case VK_FORMAT_B5G5R5A1_UNORM_PACK16:
      result = 2;
      break;
    case VK_FORMAT_A1R5G5B5_UNORM_PACK16:
      result = 2;
      break;
    case VK_FORMAT_R8_UNORM:
      result = 1;
      break;
    case VK_FORMAT_R8_SNORM:
      result = 1;
      break;
    case VK_FORMAT_R8_USCALED:
      result = 1;
      break;
    case VK_FORMAT_R8_SSCALED:
      result = 1;
      break;
    case VK_FORMAT_R8_UINT:
      result = 1;
      break;
    case VK_FORMAT_R8_SINT:
      result = 1;
      break;
    case VK_FORMAT_R8_SRGB:
      result = 1;
      break;
    case VK_FORMAT_R8G8_UNORM:
      result = 2;
      break;
    case VK_FORMAT_R8G8_SNORM:
      result = 2;
      break;
    case VK_FORMAT_R8G8_USCALED:
      result = 2;
      break;
    case VK_FORMAT_R8G8_SSCALED:
      result = 2;
      break;
    case VK_FORMAT_R8G8_UINT:
      result = 2;
      break;
    case VK_FORMAT_R8G8_SINT:
      result = 2;
      break;
    case VK_FORMAT_R8G8_SRGB:
      result = 2;
      break;
    case VK_FORMAT_R8G8B8_UNORM:
      result = 3;
      break;
    case VK_FORMAT_R8G8B8_SNORM:
      result = 3;
      break;
    case VK_FORMAT_R8G8B8_USCALED:
      result = 3;
      break;
    case VK_FORMAT_R8G8B8_SSCALED:
      result = 3;
      break;
    case VK_FORMAT_R8G8B8_UINT:
      result = 3;
      break;
    case VK_FORMAT_R8G8B8_SINT:
      result = 3;
      break;
    case VK_FORMAT_R8G8B8_SRGB:
      result = 3;
      break;
    case VK_FORMAT_B8G8R8_UNORM:
      result = 3;
      break;
    case VK_FORMAT_B8G8R8_SNORM:
      result = 3;
      break;
    case VK_FORMAT_B8G8R8_USCALED:
      result = 3;
      break;
    case VK_FORMAT_B8G8R8_SSCALED:
      result = 3;
      break;
    case VK_FORMAT_B8G8R8_UINT:
      result = 3;
      break;
    case VK_FORMAT_B8G8R8_SINT:
      result = 3;
      break;
    case VK_FORMAT_B8G8R8_SRGB:
      result = 3;
      break;
    case VK_FORMAT_R8G8B8A8_UNORM:
      result = 4;
      break;
    case VK_FORMAT_R8G8B8A8_SNORM:
      result = 4;
      break;
    case VK_FORMAT_R8G8B8A8_USCALED:
      result = 4;
      break;
    case VK_FORMAT_R8G8B8A8_SSCALED:
      result = 4;
      break;
    case VK_FORMAT_R8G8B8A8_UINT:
      result = 4;
      break;
    case VK_FORMAT_R8G8B8A8_SINT:
      result = 4;
      break;
    case VK_FORMAT_R8G8B8A8_SRGB:
      result = 4;
      break;
    case VK_FORMAT_B8G8R8A8_UNORM:
      result = 4;
      break;
    case VK_FORMAT_B8G8R8A8_SNORM:
      result = 4;
      break;
    case VK_FORMAT_B8G8R8A8_USCALED:
      result = 4;
      break;
    case VK_FORMAT_B8G8R8A8_SSCALED:
      result = 4;
      break;
    case VK_FORMAT_B8G8R8A8_UINT:
      result = 4;
      break;
    case VK_FORMAT_B8G8R8A8_SINT:
      result = 4;
      break;
    case VK_FORMAT_B8G8R8A8_SRGB:
      result = 4;
      break;
    case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
      result = 4;
      break;
    case VK_FORMAT_A8B8G8R8_SNORM_PACK32:
      result = 4;
      break;
    case VK_FORMAT_A8B8G8R8_USCALED_PACK32:
      result = 4;
      break;
    case VK_FORMAT_A8B8G8R8_SSCALED_PACK32:
      result = 4;
      break;
    case VK_FORMAT_A8B8G8R8_UINT_PACK32:
      result = 4;
      break;
    case VK_FORMAT_A8B8G8R8_SINT_PACK32:
      result = 4;
      break;
    case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
      result = 4;
      break;
    case VK_FORMAT_A2R10G10B10_UNORM_PACK32:
      result = 4;
      break;
    case VK_FORMAT_A2R10G10B10_SNORM_PACK32:
      result = 4;
      break;
    case VK_FORMAT_A2R10G10B10_USCALED_PACK32:
      result = 4;
      break;
    case VK_FORMAT_A2R10G10B10_SSCALED_PACK32:
      result = 4;
      break;
    case VK_FORMAT_A2R10G10B10_UINT_PACK32:
      result = 4;
      break;
    case VK_FORMAT_A2R10G10B10_SINT_PACK32:
      result = 4;
      break;
    case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
      result = 4;
      break;
    case VK_FORMAT_A2B10G10R10_SNORM_PACK32:
      result = 4;
      break;
    case VK_FORMAT_A2B10G10R10_USCALED_PACK32:
      result = 4;
      break;
    case VK_FORMAT_A2B10G10R10_SSCALED_PACK32:
      result = 4;
      break;
    case VK_FORMAT_A2B10G10R10_UINT_PACK32:
      result = 4;
      break;
    case VK_FORMAT_A2B10G10R10_SINT_PACK32:
      result = 4;
      break;
    case VK_FORMAT_R16_UNORM:
      result = 2;
      break;
    case VK_FORMAT_R16_SNORM:
      result = 2;
      break;
    case VK_FORMAT_R16_USCALED:
      result = 2;
      break;
    case VK_FORMAT_R16_SSCALED:
      result = 2;
      break;
    case VK_FORMAT_R16_UINT:
      result = 2;
      break;
    case VK_FORMAT_R16_SINT:
      result = 2;
      break;
    case VK_FORMAT_R16_SFLOAT:
      result = 2;
      break;
    case VK_FORMAT_R16G16_UNORM:
      result = 4;
      break;
    case VK_FORMAT_R16G16_SNORM:
      result = 4;
      break;
    case VK_FORMAT_R16G16_USCALED:
      result = 4;
      break;
    case VK_FORMAT_R16G16_SSCALED:
      result = 4;
      break;
    case VK_FORMAT_R16G16_UINT:
      result = 4;
      break;
    case VK_FORMAT_R16G16_SINT:
      result = 4;
      break;
    case VK_FORMAT_R16G16_SFLOAT:
      result = 4;
      break;
    case VK_FORMAT_R16G16B16_UNORM:
      result = 6;
      break;
    case VK_FORMAT_R16G16B16_SNORM:
      result = 6;
      break;
    case VK_FORMAT_R16G16B16_USCALED:
      result = 6;
      break;
    case VK_FORMAT_R16G16B16_SSCALED:
      result = 6;
      break;
    case VK_FORMAT_R16G16B16_UINT:
      result = 6;
      break;
    case VK_FORMAT_R16G16B16_SINT:
      result = 6;
      break;
    case VK_FORMAT_R16G16B16_SFLOAT:
      result = 6;
      break;
    case VK_FORMAT_R16G16B16A16_UNORM:
      result = 8;
      break;
    case VK_FORMAT_R16G16B16A16_SNORM:
      result = 8;
      break;
    case VK_FORMAT_R16G16B16A16_USCALED:
      result = 8;
      break;
    case VK_FORMAT_R16G16B16A16_SSCALED:
      result = 8;
      break;
    case VK_FORMAT_R16G16B16A16_UINT:
      result = 8;
      break;
    case VK_FORMAT_R16G16B16A16_SINT:
      result = 8;
      break;
    case VK_FORMAT_R16G16B16A16_SFLOAT:
      result = 8;
      break;
    case VK_FORMAT_R32_UINT:
      result = 4;
      break;
    case VK_FORMAT_R32_SINT:
      result = 4;
      break;
    case VK_FORMAT_R32_SFLOAT:
      result = 4;
      break;
    case VK_FORMAT_R32G32_UINT:
      result = 8;
      break;
    case VK_FORMAT_R32G32_SINT:
      result = 8;
      break;
    case VK_FORMAT_R32G32_SFLOAT:
      result = 8;
      break;
    case VK_FORMAT_R32G32B32_UINT:
      result = 12;
      break;
    case VK_FORMAT_R32G32B32_SINT:
      result = 12;
      break;
    case VK_FORMAT_R32G32B32_SFLOAT:
      result = 12;
      break;
    case VK_FORMAT_R32G32B32A32_UINT:
      result = 16;
      break;
    case VK_FORMAT_R32G32B32A32_SINT:
      result = 16;
      break;
    case VK_FORMAT_R32G32B32A32_SFLOAT:
      result = 16;
      break;
    case VK_FORMAT_R64_UINT:
      result = 8;
      break;
    case VK_FORMAT_R64_SINT:
      result = 8;
      break;
    case VK_FORMAT_R64_SFLOAT:
      result = 8;
      break;
    case VK_FORMAT_R64G64_UINT:
      result = 16;
      break;
    case VK_FORMAT_R64G64_SINT:
      result = 16;
      break;
    case VK_FORMAT_R64G64_SFLOAT:
      result = 16;
      break;
    case VK_FORMAT_R64G64B64_UINT:
      result = 24;
      break;
    case VK_FORMAT_R64G64B64_SINT:
      result = 24;
      break;
    case VK_FORMAT_R64G64B64_SFLOAT:
      result = 24;
      break;
    case VK_FORMAT_R64G64B64A64_UINT:
      result = 32;
      break;
    case VK_FORMAT_R64G64B64A64_SINT:
      result = 32;
      break;
    case VK_FORMAT_R64G64B64A64_SFLOAT:
      result = 32;
      break;
    case VK_FORMAT_B10G11R11_UFLOAT_PACK32:
      result = 4;
      break;
    case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32:
      result = 4;
      break;

    default:
      break;
  }
  return result;
}

std::string ToStringDescriptorType(SpvReflectDescriptorType value) {
  switch (value) {
    case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER:
      return "VK_DESCRIPTOR_TYPE_SAMPLER";
    case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
      return "VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER";
    case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
      return "VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE";
    case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE:
      return "VK_DESCRIPTOR_TYPE_STORAGE_IMAGE";
    case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
      return "VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER";
    case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
      return "VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER";
    case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
      return "VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER";
    case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:
      return "VK_DESCRIPTOR_TYPE_STORAGE_BUFFER";
    case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
      return "VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC";
    case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
      return "VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC";
    case SPV_REFLECT_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
      return "VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT";
    case SPV_REFLECT_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR:
      return "VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR";
  }
  // unhandled SpvReflectDescriptorType enum value
  return "VK_DESCRIPTOR_TYPE_???";
}

std::string ToStringSpvBuiltIn(SpvBuiltIn built_in) {
  switch (built_in) {
    case SpvBuiltInPosition:
      return "Position";
    case SpvBuiltInPointSize:
      return "PointSize";
    case SpvBuiltInClipDistance:
      return "ClipDistance";
    case SpvBuiltInCullDistance:
      return "CullDistance";
    case SpvBuiltInVertexId:
      return "VertexId";
    case SpvBuiltInInstanceId:
      return "InstanceId";
    case SpvBuiltInPrimitiveId:
      return "PrimitiveId";
    case SpvBuiltInInvocationId:
      return "InvocationId";
    case SpvBuiltInLayer:
      return "Layer";
    case SpvBuiltInViewportIndex:
      return "ViewportIndex";
    case SpvBuiltInTessLevelOuter:
      return "TessLevelOuter";
    case SpvBuiltInTessLevelInner:
      return "TessLevelInner";
    case SpvBuiltInTessCoord:
      return "TessCoord";
    case SpvBuiltInPatchVertices:
      return "PatchVertices";
    case SpvBuiltInFragCoord:
      return "FragCoord";
    case SpvBuiltInPointCoord:
      return "PointCoord";
    case SpvBuiltInFrontFacing:
      return "FrontFacing";
    case SpvBuiltInSampleId:
      return "SampleId";
    case SpvBuiltInSamplePosition:
      return "SamplePosition";
    case SpvBuiltInSampleMask:
      return "SampleMask";
    case SpvBuiltInFragDepth:
      return "FragDepth";
    case SpvBuiltInHelperInvocation:
      return "HelperInvocation";
    case SpvBuiltInNumWorkgroups:
      return "NumWorkgroups";
    case SpvBuiltInWorkgroupSize:
      return "WorkgroupSize";
    case SpvBuiltInWorkgroupId:
      return "WorkgroupId";
    case SpvBuiltInLocalInvocationId:
      return "LocalInvocationId";
    case SpvBuiltInGlobalInvocationId:
      return "GlobalInvocationId";
    case SpvBuiltInLocalInvocationIndex:
      return "LocalInvocationIndex";
    case SpvBuiltInWorkDim:
      return "WorkDim";
    case SpvBuiltInGlobalSize:
      return "GlobalSize";
    case SpvBuiltInEnqueuedWorkgroupSize:
      return "EnqueuedWorkgroupSize";
    case SpvBuiltInGlobalOffset:
      return "GlobalOffset";
    case SpvBuiltInGlobalLinearId:
      return "GlobalLinearId";
    case SpvBuiltInSubgroupSize:
      return "SubgroupSize";
    case SpvBuiltInSubgroupMaxSize:
      return "SubgroupMaxSize";
    case SpvBuiltInNumSubgroups:
      return "NumSubgroups";
    case SpvBuiltInNumEnqueuedSubgroups:
      return "NumEnqueuedSubgroups";
    case SpvBuiltInSubgroupId:
      return "SubgroupId";
    case SpvBuiltInSubgroupLocalInvocationId:
      return "SubgroupLocalInvocationId";
    case SpvBuiltInVertexIndex:
      return "VertexIndex";
    case SpvBuiltInInstanceIndex:
      return "InstanceIndex";
    case SpvBuiltInSubgroupEqMaskKHR:
      return "SubgroupEqMaskKHR";
    case SpvBuiltInSubgroupGeMaskKHR:
      return "SubgroupGeMaskKHR";
    case SpvBuiltInSubgroupGtMaskKHR:
      return "SubgroupGtMaskKHR";
    case SpvBuiltInSubgroupLeMaskKHR:
      return "SubgroupLeMaskKHR";
    case SpvBuiltInSubgroupLtMaskKHR:
      return "SubgroupLtMaskKHR";
    case SpvBuiltInBaseVertex:
      return "BaseVertex";
    case SpvBuiltInBaseInstance:
      return "BaseInstance";
    case SpvBuiltInDrawIndex:
      return "DrawIndex";
    case SpvBuiltInDeviceIndex:
      return "DeviceIndex";
    case SpvBuiltInViewIndex:
      return "ViewIndex";
    case SpvBuiltInBaryCoordNoPerspAMD:
      return "BaryCoordNoPerspAMD";
    case SpvBuiltInBaryCoordNoPerspCentroidAMD:
      return "BaryCoordNoPerspCentroidAMD";
    case SpvBuiltInBaryCoordNoPerspSampleAMD:
      return "BaryCoordNoPerspSampleAMD";
    case SpvBuiltInBaryCoordSmoothAMD:
      return "BaryCoordSmoothAMD";
    case SpvBuiltInBaryCoordSmoothCentroidAMD:
      return "BaryCoordSmoothCentroidAMD";
    case SpvBuiltInBaryCoordSmoothSampleAMD:
      return "BaryCoordSmoothSampleAMD";
    case SpvBuiltInBaryCoordPullModelAMD:
      return "BaryCoordPullModelAMD";
    case SpvBuiltInFragStencilRefEXT:
      return "FragStencilRefEXT";
    case SpvBuiltInViewportMaskNV:
      return "ViewportMaskNV";
    case SpvBuiltInSecondaryPositionNV:
      return "SecondaryPositionNV";
    case SpvBuiltInSecondaryViewportMaskNV:
      return "SecondaryViewportMaskNV";
    case SpvBuiltInPositionPerViewNV:
      return "PositionPerViewNV";
    case SpvBuiltInViewportMaskPerViewNV:
      return "ViewportMaskPerViewNV";
    case SpvBuiltInLaunchIdKHR:
      return "InLaunchIdKHR";
    case SpvBuiltInLaunchSizeKHR:
      return "InLaunchSizeKHR";
    case SpvBuiltInWorldRayOriginKHR:
      return "InWorldRayOriginKHR";
    case SpvBuiltInWorldRayDirectionKHR:
      return "InWorldRayDirectionKHR";
    case SpvBuiltInObjectRayOriginKHR:
      return "InObjectRayOriginKHR";
    case SpvBuiltInObjectRayDirectionKHR:
      return "InObjectRayDirectionKHR";
    case SpvBuiltInRayTminKHR:
      return "InRayTminKHR";
    case SpvBuiltInRayTmaxKHR:
      return "InRayTmaxKHR";
    case SpvBuiltInInstanceCustomIndexKHR:
      return "InInstanceCustomIndexKHR";
    case SpvBuiltInObjectToWorldKHR:
      return "InObjectToWorldKHR";
    case SpvBuiltInWorldToObjectKHR:
      return "InWorldToObjectKHR";
    case SpvBuiltInHitTNV:
      return "InHitTNV";
    case SpvBuiltInHitKindKHR:
      return "InHitKindKHR";
    case SpvBuiltInIncomingRayFlagsKHR:
      return "InIncomingRayFlagsKHR";
    case SpvBuiltInRayGeometryIndexKHR:
      return "InRayGeometryIndexKHR";

    case SpvBuiltInMax:
    default:
      break;
  }
  // unhandled SpvBuiltIn enum value
  std::stringstream ss;
  ss << "??? (" << built_in << ")";
  return ss.str();
}

static std::string ToStringScalarType(const SpvReflectTypeDescription& type) {
  switch (type.op) {
    case SpvOpTypeVoid: {
      return "void";
      break;
    }
    case SpvOpTypeBool: {
      return "bool";
      break;
    }
    case SpvOpTypeInt: {
      if (type.traits.numeric.scalar.signedness)
        return "int";
      else
        return "uint";
    }
    case SpvOpTypeFloat: {
      switch (type.traits.numeric.scalar.width) {
        case 32:
          return "float";
        case 64:
          return "double";
        default:
          break;
      }
      break;
    }
    case SpvOpTypeStruct: {
      return "struct";
    }
    default: {
      break;
    }
  }
  return "";
}

static std::string ToStringGlslType(const SpvReflectTypeDescription& type) {
  switch (type.op) {
    case SpvOpTypeVector: {
      switch (type.traits.numeric.scalar.width) {
        case 32: {
          switch (type.traits.numeric.vector.component_count) {
            case 2:
              return "vec2";
            case 3:
              return "vec3";
            case 4:
              return "vec4";
          }
        } break;

        case 64: {
          switch (type.traits.numeric.vector.component_count) {
            case 2:
              return "dvec2";
            case 3:
              return "dvec3";
            case 4:
              return "dvec4";
          }
        } break;
      }
    } break;
    default:
      break;
  }
  return ToStringScalarType(type);
}

static std::string ToStringHlslType(const SpvReflectTypeDescription& type) {
  switch (type.op) {
    case SpvOpTypeVector: {
      switch (type.traits.numeric.scalar.width) {
        case 32: {
          switch (type.traits.numeric.vector.component_count) {
            case 2:
              return "float2";
            case 3:
              return "float3";
            case 4:
              return "float4";
          }
        } break;

        case 64: {
          switch (type.traits.numeric.vector.component_count) {
            case 2:
              return "double2";
            case 3:
              return "double3";
            case 4:
              return "double4";
          }
        } break;
      }
    } break;

    default:
      break;
  }
  return ToStringScalarType(type);
}

std::string ToStringType(SpvSourceLanguage src_lang,
                         const SpvReflectTypeDescription& type) {
  if (src_lang == SpvSourceLanguageHLSL) {
    return ToStringHlslType(type);
  }

  return ToStringGlslType(type);
}


std::string ToStringFormat(SpvReflectFormat fmt) {
  switch (fmt) {
    case SPV_REFLECT_FORMAT_UNDEFINED:
      return "VK_FORMAT_UNDEFINED";
    case SPV_REFLECT_FORMAT_R32_UINT:
      return "VK_FORMAT_R32_UINT";
    case SPV_REFLECT_FORMAT_R32_SINT:
      return "VK_FORMAT_R32_SINT";
    case SPV_REFLECT_FORMAT_R32_SFLOAT:
      return "VK_FORMAT_R32_SFLOAT";
    case SPV_REFLECT_FORMAT_R32G32_UINT:
      return "VK_FORMAT_R32G32_UINT";
    case SPV_REFLECT_FORMAT_R32G32_SINT:
      return "VK_FORMAT_R32G32_SINT";
    case SPV_REFLECT_FORMAT_R32G32_SFLOAT:
      return "VK_FORMAT_R32G32_SFLOAT";
    case SPV_REFLECT_FORMAT_R32G32B32_UINT:
      return "VK_FORMAT_R32G32B32_UINT";
    case SPV_REFLECT_FORMAT_R32G32B32_SINT:
      return "VK_FORMAT_R32G32B32_SINT";
    case SPV_REFLECT_FORMAT_R32G32B32_SFLOAT:
      return "VK_FORMAT_R32G32B32_SFLOAT";
    case SPV_REFLECT_FORMAT_R32G32B32A32_UINT:
      return "VK_FORMAT_R32G32B32A32_UINT";
    case SPV_REFLECT_FORMAT_R32G32B32A32_SINT:
      return "VK_FORMAT_R32G32B32A32_SINT";
    case SPV_REFLECT_FORMAT_R32G32B32A32_SFLOAT:
      return "VK_FORMAT_R32G32B32A32_SFLOAT";
    case SPV_REFLECT_FORMAT_R64_UINT:
      return "VK_FORMAT_R64_UINT";
    case SPV_REFLECT_FORMAT_R64_SINT:
      return "VK_FORMAT_R64_SINT";
    case SPV_REFLECT_FORMAT_R64_SFLOAT:
      return "VK_FORMAT_R64_SFLOAT";
    case SPV_REFLECT_FORMAT_R64G64_UINT:
      return "VK_FORMAT_R64G64_UINT";
    case SPV_REFLECT_FORMAT_R64G64_SINT:
      return "VK_FORMAT_R64G64_SINT";
    case SPV_REFLECT_FORMAT_R64G64_SFLOAT:
      return "VK_FORMAT_R64G64_SFLOAT";
    case SPV_REFLECT_FORMAT_R64G64B64_UINT:
      return "VK_FORMAT_R64G64B64_UINT";
    case SPV_REFLECT_FORMAT_R64G64B64_SINT:
      return "VK_FORMAT_R64G64B64_SINT";
    case SPV_REFLECT_FORMAT_R64G64B64_SFLOAT:
      return "VK_FORMAT_R64G64B64_SFLOAT";
    case SPV_REFLECT_FORMAT_R64G64B64A64_UINT:
      return "VK_FORMAT_R64G64B64A64_UINT";
    case SPV_REFLECT_FORMAT_R64G64B64A64_SINT:
      return "VK_FORMAT_R64G64B64A64_SINT";
    case SPV_REFLECT_FORMAT_R64G64B64A64_SFLOAT:
      return "VK_FORMAT_R64G64B64A64_SFLOAT";
  }
  // unhandled SpvReflectFormat enum value
  return "VK_FORMAT_???";
}

void PrintModuleInfo(std::ostream& os, const SpvReflectShaderModule& obj) {
  os << "entry point     : " << obj.entry_point_name << "\n";
  os << "source lang     : " << spvReflectSourceLanguage(obj.source_language)
     << "\n";
  os << "source lang ver : " << obj.source_language_version << "\n";
  if (obj.source_language == SpvSourceLanguageHLSL) {
    os << "stage           : ";
    switch (obj.shader_stage) {
      default:
        break;
      case SPV_REFLECT_SHADER_STAGE_VERTEX_BIT:
        os << "VS";
        break;
      case SPV_REFLECT_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
        os << "HS";
        break;
      case SPV_REFLECT_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
        os << "DS";
        break;
      case SPV_REFLECT_SHADER_STAGE_GEOMETRY_BIT:
        os << "GS";
        break;
      case SPV_REFLECT_SHADER_STAGE_FRAGMENT_BIT:
        os << "PS";
        break;
      case SPV_REFLECT_SHADER_STAGE_COMPUTE_BIT:
        os << "CS";
        break;
    }
  }
}

void PrintDescriptorSet(std::ostream& os, const SpvReflectDescriptorSet& obj,
                        const char* indent) {
  const char* t = indent;
  std::string tt = std::string(indent) + "  ";
  std::string ttttt = std::string(indent) + "    ";

  os << t << "set           : " << obj.set << "\n";
  os << t << "binding count : " << obj.binding_count;
  os << "\n";
  for (uint32_t i = 0; i < obj.binding_count; ++i) {
    const SpvReflectDescriptorBinding& binding = *obj.bindings[i];
    os << tt << i << ":"
       << "\n";
    PrintDescriptorBinding(os, binding, false, ttttt.c_str());
    if (i < (obj.binding_count - 1)) {
      os << "\n";
    }
  }
}

void PrintDescriptorBinding(std::ostream& os,
                            const SpvReflectDescriptorBinding& obj,
                            bool write_set, const char* indent) {
  const char* t = indent;
  os << t << "binding : " << obj.binding << "\n";
  if (write_set) {
    os << t << "set     : " << obj.set << "\n";
  }
  os << t << "type    : " << ToStringDescriptorType(obj.descriptor_type)
     << "\n";

  // array
  if (obj.array.dims_count > 0) {
    os << t << "array   : ";
    for (uint32_t dim_index = 0; dim_index < obj.array.dims_count;
         ++dim_index) {
      os << "[" << obj.array.dims[dim_index] << "]";
    }
    os << "\n";
  }

  // counter
  if (obj.uav_counter_binding != nullptr) {
    os << t << "counter : ";
    os << "(";
    os << "set=" << obj.uav_counter_binding->set << ", ";
    os << "binding=" << obj.uav_counter_binding->binding << ", ";
    os << "name=" << obj.uav_counter_binding->name;
    os << ");";
    os << "\n";
  }

  os << t << "name    : " << obj.name;
  if ((obj.type_description->type_name != nullptr) &&
      (strlen(obj.type_description->type_name) > 0)) {
    os << " "
       << "(" << obj.type_description->type_name << ")";
  }
}

void PrintInterfaceVariable(std::ostream& os, SpvSourceLanguage src_lang,
                            const SpvReflectInterfaceVariable& obj,
                            const char* indent) {
  const char* t = indent;
  os << t << "location  : ";
  if (obj.decoration_flags & SPV_REFLECT_DECORATION_BUILT_IN) {
    os << ToStringSpvBuiltIn(obj.built_in) << " "
       << "(built-in)";
  } else {
    os << obj.location;
  }
  os << "\n";
  if (obj.semantic != nullptr) {
    os << t << "semantic  : " << obj.semantic << "\n";
  }
  os << t << "type      : " << ToStringType(src_lang, *obj.type_description)
     << "\n";
  os << t << "format    : " << ToStringFormat(obj.format) << "\n";
  os << t << "qualifier : ";
  if (obj.decoration_flags & SPV_REFLECT_DECORATION_FLAT) {
    os << "flat";
  } else if (obj.decoration_flags & SPV_REFLECT_DECORATION_NOPERSPECTIVE) {
    os << "noperspective";
  }
  os << "\n";

  os << t << "name      : " << obj.name;
  if ((obj.type_description->type_name != nullptr) &&
      (strlen(obj.type_description->type_name) > 0)) {
    os << " "
       << "(" << obj.type_description->type_name << ")";
  }
}

void foo( const std::vector< char > &i_bytes )
{
    SpvReflectShaderModule module = {};
    SpvReflectResult result =
        spvReflectCreateShaderModule( i_bytes.size(), i_bytes.data(), &module);
    assert(result == SPV_REFLECT_RESULT_SUCCESS);
    
      uint32_t count = 0;
      result = spvReflectEnumerateInputVariables(&module, &count, NULL);
      assert(result == SPV_REFLECT_RESULT_SUCCESS);

      std::vector<SpvReflectInterfaceVariable*> input_vars(count);
      result =
          spvReflectEnumerateInputVariables(&module, &count, input_vars.data());
      assert(result == SPV_REFLECT_RESULT_SUCCESS);

      count = 0;
      result = spvReflectEnumerateOutputVariables(&module, &count, NULL);
      assert(result == SPV_REFLECT_RESULT_SUCCESS);

      std::vector<SpvReflectInterfaceVariable*> output_vars(count);
      result =
          spvReflectEnumerateOutputVariables(&module, &count, output_vars.data());
      assert(result == SPV_REFLECT_RESULT_SUCCESS);


    if (module.shader_stage == SPV_REFLECT_SHADER_STAGE_VERTEX_BIT) {
        // Demonstrates how to generate all necessary data structures to populate
        // a VkPipelineVertexInputStateCreateInfo structure, given the module's
        // expected input variables.
        //
        // Simplifying assumptions:
        // - All vertex input attributes are sourced from a single vertex buffer,
        //   bound to VB slot 0.
        // - Each vertex's attribute are laid out in ascending order by location.
        // - The format of each attribute matches its usage in the shader;
        //   float4 -> VK_FORMAT_R32G32B32A32_FLOAT, etc. No attribute compression
        //   is applied.
        // - All attributes are provided per-vertex, not per-instance.
        VkVertexInputBindingDescription binding_description = {};
        binding_description.binding = 0;
        binding_description.stride = 0;  // computed below
        binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = {
            VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
        std::vector<VkVertexInputAttributeDescription> attribute_descriptions;
        attribute_descriptions.reserve(input_vars.size());
        for (size_t i_var = 0; i_var < input_vars.size(); ++i_var) {
          const SpvReflectInterfaceVariable& refl_var = *(input_vars[i_var]);
          // ignore built-in variables
          if (refl_var.decoration_flags & SPV_REFLECT_DECORATION_BUILT_IN) {
            continue;
          }
          VkVertexInputAttributeDescription attr_desc{};
          attr_desc.location = refl_var.location;
          attr_desc.binding = binding_description.binding;
          attr_desc.format = static_cast<VkFormat>(refl_var.format);
          attr_desc.offset = 0;  // final offset computed below after sorting.
          attribute_descriptions.push_back(attr_desc);
        }
        // Sort attributes by location
        std::sort(std::begin(attribute_descriptions),
                  std::end(attribute_descriptions),
                  [](const VkVertexInputAttributeDescription& a,
                     const VkVertexInputAttributeDescription& b) {
                    return a.location < b.location;
                  });
        // Compute final offsets of each attribute, and total vertex stride.
        for (auto& attribute : attribute_descriptions) {
          uint32_t format_size = formatSize(attribute.format);
          attribute.offset = binding_description.stride;
          binding_description.stride += format_size;
        }
        // Nothing further is done with attribute_descriptions or
        // binding_description in this sample. A real application would probably
        // derive this information from its mesh format(s); a similar mechanism
        // could be used to ensure mesh/shader compatibility.
      }

      // Log the interface variables to stdout
      const char* t = "  ";
      const char* tt = "    ";

      PrintModuleInfo(std::cout, module);
      std::cout << "\n\n";

      std::cout << "Input variables:"
                << "\n";
      for (size_t index = 0; index < input_vars.size(); ++index) {
        auto p_var = input_vars[index];

        // input variables can also be retrieved directly from the module, by
        // location (unless the location is (uint32_t)-1, as is the case with
        // built-in inputs)
        auto p_var2 =
            spvReflectGetInputVariableByLocation(&module, p_var->location, &result);
        if (p_var->location == UINT32_MAX) {
          assert(result == SPV_REFLECT_RESULT_ERROR_ELEMENT_NOT_FOUND);
          assert(p_var2 == nullptr);
        } else {
          assert(result == SPV_REFLECT_RESULT_SUCCESS);
          assert(p_var == p_var2);
        }
        (void)p_var2;

        // input variables can also be retrieved directly from the module, by
        // semantic (if present)
        p_var2 =
            spvReflectGetInputVariableBySemantic(&module, p_var->semantic, &result);
        if (!p_var->semantic) {
          assert(result == SPV_REFLECT_RESULT_ERROR_NULL_POINTER);
          assert(p_var2 == nullptr);
        } else if (p_var->semantic[0] != '\0') {
          assert(result == SPV_REFLECT_RESULT_ERROR_ELEMENT_NOT_FOUND);
          assert(p_var2 == nullptr);
        } else {
          assert(result == SPV_REFLECT_RESULT_SUCCESS);
          assert(p_var == p_var2);
        }
        (void)p_var2;

        std::cout << t << index << ":"
                  << "\n";
        PrintInterfaceVariable(std::cout, module.source_language, *p_var, tt);
        std::cout << "\n\n";
      }

      std::cout << "Output variables:"
                << "\n";
      for (size_t index = 0; index < output_vars.size(); ++index) {
        auto p_var = output_vars[index];

        // output variables can also be retrieved directly from the module, by
        // location (unless the location is (uint32_t)-1, as is the case with
        // built-in outputs)
        auto p_var2 = spvReflectGetOutputVariableByLocation(
            &module, p_var->location, &result);
        if (p_var->location == UINT32_MAX) {
          assert(result == SPV_REFLECT_RESULT_ERROR_ELEMENT_NOT_FOUND);
          assert(p_var2 == nullptr);
        } else {
          assert(result == SPV_REFLECT_RESULT_SUCCESS);
          assert(p_var == p_var2);
        }
        (void)p_var2;

        // output variables can also be retrieved directly from the module, by
        // semantic (if present)
        p_var2 = spvReflectGetOutputVariableBySemantic(&module, p_var->semantic,
                                                       &result);
        if (!p_var->semantic) {
          assert(result == SPV_REFLECT_RESULT_ERROR_NULL_POINTER);
          assert(p_var2 == nullptr);
        } else if (p_var->semantic[0] != '\0') {
          assert(result == SPV_REFLECT_RESULT_ERROR_ELEMENT_NOT_FOUND);
          assert(p_var2 == nullptr);
        } else {
          assert(result == SPV_REFLECT_RESULT_SUCCESS);
          assert(p_var == p_var2);
        }
        (void)p_var2;

        std::cout << t << index << ":"
                  << "\n";
        PrintInterfaceVariable(std::cout, module.source_language, *p_var, tt);
        std::cout << "\n\n";
      }

      spvReflectDestroyShaderModule(&module);
}

void readFile( const std::string &i_filename, std::vector< char > &o_bytes )
{
    std::ifstream file( i_filename, std::ios::ate | std::ios::binary );

    if ( !file.is_open() )
    {
        throw std::runtime_error( "Failed to open file!" );
    }
    
    const size_t fileSize = static_cast< size_t >( file.tellg() );
    
    file.seekg( 0 );
    
    o_bytes.resize( fileSize );
    file.read( o_bytes.data(), fileSize );
    
    file.close();
}
    
ShaderStage::ShaderStage( const std::string &i_path )
{    
    readFile( i_path, m_bytes );
    
    if ( spvReflectCreateShaderModule( m_bytes.size(), m_bytes.data(), &m_spvModule ) != SPV_REFLECT_RESULT_SUCCESS )
    {
        throw std::runtime_error( "Failed to create spv shader module!" );
    }
}

ShaderStage::~ShaderStage()
{
    spvReflectDestroyShaderModule( &m_spvModule );
}

std::string ShaderStage::getEntryPoint() const
{
    return m_spvModule.entry_point_name;
}

void ShaderStage::getDescriptorSetLayouts( std::vector< DescriptorSetLayoutData > &o_layouts ) const
{
    // Taken from spv examples
    uint32_t count = 0;
    if ( spvReflectEnumerateDescriptorSets( &m_spvModule, &count, nullptr ) != SPV_REFLECT_RESULT_SUCCESS )
    {
        std::cerr << "Warning: Unable to query descriptor sets." << std::endl;
        return;
    }

    o_layouts.resize( count );
    
    std::vector< SpvReflectDescriptorSet* > descriptorSets( count );
    spvReflectEnumerateDescriptorSets( &m_spvModule, &count, descriptorSets.data() );

    for ( size_t setIdx = 0; setIdx < descriptorSets.size(); setIdx++ )
    {
        const SpvReflectDescriptorSet &descriptorSet = *( descriptorSets[ setIdx ] );
        DescriptorSetLayoutData &layout = o_layouts[ setIdx ];

        layout.number = descriptorSet.set;
        layout.bindings.resize( descriptorSet.binding_count );
        
        for ( uint32_t bindingIdx = 0; bindingIdx < layout.bindings.size(); bindingIdx++ )
        {
            const SpvReflectDescriptorBinding &setBinding = *( descriptorSet.bindings[ bindingIdx ] );
            VkDescriptorSetLayoutBinding &layoutBinding = layout.bindings[ bindingIdx ];
            layoutBinding.binding = setBinding.binding;
            layoutBinding.descriptorType = static_cast< VkDescriptorType >( setBinding.descriptor_type );
            layoutBinding.descriptorCount = 1;
            for ( uint32_t dimIdx = 0; dimIdx < setBinding.array.dims_count; dimIdx++ )
            {
                layoutBinding.descriptorCount *= setBinding.array.dims[ dimIdx ];
            }
            layoutBinding.stageFlags = static_cast< VkShaderStageFlagBits >( m_spvModule.shader_stage );
        }
    }
}

VkShaderStageFlagBits ShaderStage::getStage() const
{
    switch ( m_spvModule.shader_stage ) {
        case SPV_REFLECT_SHADER_STAGE_VERTEX_BIT:
            return VK_SHADER_STAGE_VERTEX_BIT;
        case SPV_REFLECT_SHADER_STAGE_FRAGMENT_BIT:
            return VK_SHADER_STAGE_FRAGMENT_BIT;
        case SPV_REFLECT_SHADER_STAGE_COMPUTE_BIT:
            return VK_SHADER_STAGE_COMPUTE_BIT;
        default:
            break;
    }
    
    return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
}

const std::vector< char > & ShaderStage::getBytes() const
{
    return m_bytes;
}

} // namespace marlin
