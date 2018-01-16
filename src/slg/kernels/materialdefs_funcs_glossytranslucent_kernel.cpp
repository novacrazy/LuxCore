#include <string>
namespace slg { namespace ocl {
std::string KernelSource_materialdefs_funcs_glossytranslucent = 
"#line 2 \"materialdefs_funcs_glossytranslucent.cl\"\n"
"\n"
"/***************************************************************************\n"
" * Copyright 1998-2018 by authors (see AUTHORS.txt)                        *\n"
" *                                                                         *\n"
" *   This file is part of LuxCoreRender.                                   *\n"
" *                                                                         *\n"
" * Licensed under the Apache License, Version 2.0 (the \"License\");         *\n"
" * you may not use this file except in compliance with the License.        *\n"
" * You may obtain a copy of the License at                                 *\n"
" *                                                                         *\n"
" *     http://www.apache.org/licenses/LICENSE-2.0                          *\n"
" *                                                                         *\n"
" * Unless required by applicable law or agreed to in writing, software     *\n"
" * distributed under the License is distributed on an \"AS IS\" BASIS,       *\n"
" * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.*\n"
" * See the License for the specific language governing permissions and     *\n"
" * limitations under the License.                                          *\n"
" ***************************************************************************/\n"
"\n"
"//------------------------------------------------------------------------------\n"
"// GlossyTranslucent material\n"
"//\n"
"// LuxRender GlossyTranslucent material porting.\n"
"//------------------------------------------------------------------------------\n"
"\n"
"#if defined (PARAM_ENABLE_MAT_GLOSSYTRANSLUCENT)\n"
"\n"
"BSDFEvent GlossyTranslucentMaterial_GetEventTypes() {\n"
"	return GLOSSY | DIFFUSE | REFLECT | TRANSMIT;\n"
"}\n"
"\n"
"float3 GlossyTranslucentMaterial_Evaluate(\n"
"		__global HitPoint *hitPoint, const float3 lightDir, const float3 eyeDir,\n"
"		BSDFEvent *event, float *directPdfW,\n"
"#if defined(PARAM_ENABLE_MAT_GLOSSYTRANSLUCENT_INDEX)\n"
"		const float i, const float i_bf,\n"
"#endif\n"
"		const float nuVal, const float nuVal_bf,\n"
"#if defined(PARAM_ENABLE_MAT_GLOSSYTRANSLUCENT_ANISOTROPIC)\n"
"		const float nvVal, const float nvVal_bf,\n"
"#endif\n"
"#if defined(PARAM_ENABLE_MAT_GLOSSYTRANSLUCENT_ABSORPTION)\n"
"		const float3 kaVal, const float3 kaVal_bf,\n"
"		const float d, const float d_bf,\n"
"#endif\n"
"#if defined(PARAM_ENABLE_MAT_GLOSSYTRANSLUCENT_MULTIBOUNCE)\n"
"		const int multibounceVal, const int multibounceVal_bf,\n"
"#endif\n"
"		const float3 kdVal, const float3 ktVal, const float3 ksVal, const float3 ksVal_bf) {\n"
"	const float3 fixedDir = eyeDir;\n"
"	const float3 sampledDir = lightDir;\n"
"\n"
"	const float cosi = fabs(lightDir.z);\n"
"	const float coso = fabs(eyeDir.z);\n"
"\n"
"	// Note: this is the same side test used by matte translucent material and\n"
"	// it is different from the CPU test because HitPoint::dpdu and HitPoint::dpdv\n"
"	// are not available here without bump mapping.\n"
"	const float sideTest = CosTheta(lightDir) * CosTheta(eyeDir);\n"
"\n"
"	if (sideTest < -DEFAULT_COS_EPSILON_STATIC) {\n"
"		// Transmission\n"
"		*event = DIFFUSE | TRANSMIT;\n"
"\n"
"		if (directPdfW)\n"
"			*directPdfW = fabs(sampledDir.z) * (M_1_PI_F * .5f);\n"
"\n"
"		const float3 H = normalize((float3)(lightDir.x + eyeDir.x, lightDir.y + eyeDir.y,\n"
"			lightDir.z - eyeDir.z));\n"
"		const float u = fabs(dot(lightDir, H));\n"
"		float3 ks = ksVal;\n"
"#if defined(PARAM_ENABLE_MAT_GLOSSYTRANSLUCENT_INDEX)\n"
"		if (i > 0.f) {\n"
"			const float ti = (i - 1.f) / (i + 1.f);\n"
"			ks *= ti * ti;\n"
"		}\n"
"#endif\n"
"		ks = Spectrum_Clamp(ks);\n"
"		const float3 S1 = FresnelSchlick_Evaluate(ks, u);\n"
"\n"
"		ks = ksVal_bf;\n"
"#if defined(PARAM_ENABLE_MAT_GLOSSYTRANSLUCENT_INDEX)\n"
"		if (i_bf > 0.f) {\n"
"			const float ti = (i_bf - 1.f) / (i_bf + 1.f);\n"
"			ks *= ti * ti;\n"
"		}\n"
"#endif\n"
"		ks = Spectrum_Clamp(ks);\n"
"		const float3 S2 = FresnelSchlick_Evaluate(ks, u);\n"
"		float3 S = Spectrum_Sqrt((WHITE - S1) * (WHITE - S2));\n"
"#if defined(PARAM_ENABLE_MAT_GLOSSYTRANSLUCENT_ABSORPTION)\n"
"		if (lightDir.z > 0.f) {\n"
"			S *= Spectrum_Exp(Spectrum_Clamp(kaVal) * -(d / cosi) +\n"
"				Spectrum_Clamp(kaVal_bf) * -(d_bf / coso));\n"
"		} else {\n"
"			S *= Spectrum_Exp(Spectrum_Clamp(kaVal) * -(d / coso) +\n"
"				Spectrum_Clamp(kaVal_bf) * -(d_bf / cosi));\n"
"		}\n"
"#endif\n"
"\n"
"		return (M_1_PI_F * cosi) * S * Spectrum_Clamp(ktVal) *\n"
"			(WHITE - Spectrum_Clamp(kdVal));\n"
"	} else if (sideTest > DEFAULT_COS_EPSILON_STATIC) {\n"
"		// Reflection\n"
"		*event = GLOSSY | REFLECT;\n"
"\n"
"		const float3 baseF = Spectrum_Clamp(kdVal) * M_1_PI_F * cosi;\n"
"		float3 ks;\n"
"#if defined(PARAM_ENABLE_MAT_GLOSSYTRANSLUCENT_INDEX)\n"
"		float index;\n"
"#endif\n"
"		float u;\n"
"#if defined(PARAM_ENABLE_MAT_GLOSSYTRANSLUCENT_ANISOTROPIC)\n"
"		float v;\n"
"#endif\n"
"#if defined(PARAM_ENABLE_MAT_GLOSSYTRANSLUCENT_ABSORPTION)\n"
"		float3 alpha;\n"
"		float depth;\n"
"#endif\n"
"#if defined(PARAM_ENABLE_MAT_GLOSSYTRANSLUCENT_MULTIBOUNCE)\n"
"		int mbounce;\n"
"#else\n"
"		int mbounce = 0;\n"
"#endif\n"
"		if (eyeDir.z >= 0.f) {\n"
"			ks = ksVal;\n"
"#if defined(PARAM_ENABLE_MAT_GLOSSYTRANSLUCENT_INDEX)\n"
"			index = i;\n"
"#endif\n"
"			u = clamp(nuVal, 1e-9f, 1.f);\n"
"#if defined(PARAM_ENABLE_MAT_GLOSSYTRANSLUCENT_ANISOTROPIC)\n"
"			v = clamp(nvVal, 1e-9f, 1.f);\n"
"#endif\n"
"#if defined(PARAM_ENABLE_MAT_GLOSSYTRANSLUCENT_ABSORPTION)\n"
"			alpha = Spectrum_Clamp(kaVal);\n"
"			depth = d;\n"
"#endif\n"
"#if defined(PARAM_ENABLE_MAT_GLOSSYTRANSLUCENT_MULTIBOUNCE)\n"
"			mbounce = multibounceVal;\n"
"#endif\n"
"		} else {\n"
"			ks = ksVal_bf;\n"
"#if defined(PARAM_ENABLE_MAT_GLOSSYTRANSLUCENT_INDEX)\n"
"			index = i_bf;\n"
"#endif\n"
"			u = clamp(nuVal_bf, 1e-9f, 1.f);\n"
"#if defined(PARAM_ENABLE_MAT_GLOSSYTRANSLUCENT_ANISOTROPIC)\n"
"			v = clamp(nvVal_bf, 1e-9f, 1.f);\n"
"#endif\n"
"#if defined(PARAM_ENABLE_MAT_GLOSSYTRANSLUCENT_ABSORPTION)\n"
"			alpha = Spectrum_Clamp(kaVal_bf);\n"
"			depth = d_bf;\n"
"#endif\n"
"#if defined(PARAM_ENABLE_MAT_GLOSSYTRANSLUCENT_MULTIBOUNCE)\n"
"			mbounce = multibounceVal_bf;\n"
"#endif\n"
"		}\n"
"\n"
"#if defined(PARAM_ENABLE_MAT_GLOSSYTRANSLUCENT_INDEX)\n"
"		if (index > 0.f) {\n"
"			const float ti = (index - 1.f) / (index + 1.f);\n"
"			ks *= ti * ti;\n"
"		}\n"
"#endif\n"
"		ks = Spectrum_Clamp(ks);\n"
"\n"
"#if defined(PARAM_ENABLE_MAT_GLOSSYTRANSLUCENT_ANISOTROPIC)\n"
"		const float u2 = u * u;\n"
"		const float v2 = v * v;\n"
"		const float anisotropy = (u2 < v2) ? (1.f - u2 / v2) : u2 > 0.f ? (v2 / u2 - 1.f) : 0.f;\n"
"		const float roughness = u * v;\n"
"#else\n"
"		const float anisotropy = 0.f;\n"
"		const float roughness = u * u;\n"
"#endif\n"
"\n"
"		if (directPdfW) {\n"
"			const float wCoating = SchlickBSDF_CoatingWeight(ks, fixedDir);\n"
"			const float wBase = 1.f - wCoating;\n"
"\n"
"			*directPdfW = .5f * (wBase * fabs(sampledDir.z * M_1_PI_F) +\n"
"				wCoating * SchlickBSDF_CoatingPdf(roughness, anisotropy, fixedDir, sampledDir));\n"
"		}\n"
"\n"
"		// Absorption\n"
"#if defined(PARAM_ENABLE_MAT_GLOSSYTRANSLUCENT_ABSORPTION)\n"
"		const float3 absorption = CoatingAbsorption(cosi, coso, alpha, depth);\n"
"#else\n"
"		const float3 absorption = WHITE;\n"
"#endif\n"
"\n"
"		// Coating fresnel factor\n"
"		const float3 H = normalize(fixedDir + sampledDir);\n"
"		const float3 S = FresnelSchlick_Evaluate(ks, fabs(dot(sampledDir, H)));\n"
"\n"
"		const float3 coatingF = SchlickBSDF_CoatingF(ks, roughness, anisotropy, mbounce, fixedDir, sampledDir);\n"
"\n"
"		// Blend in base layer Schlick style\n"
"		// assumes coating bxdf takes fresnel factor S into account\n"
"\n"
"		return coatingF + absorption * (WHITE - S) * baseF;\n"
"	} else\n"
"		return BLACK;\n"
"}\n"
"\n"
"float3 GlossyTranslucentMaterial_Sample(\n"
"		__global HitPoint *hitPoint, const float3 fixedDir, float3 *sampledDir,\n"
"		const float u0, const float u1,\n"
"#if defined(PARAM_HAS_PASSTHROUGH)\n"
"		const float passThroughEvent,\n"
"#endif\n"
"		float *pdfW, float *cosSampledDir, BSDFEvent *event,\n"
"#if defined(PARAM_ENABLE_MAT_GLOSSYTRANSLUCENT_INDEX)\n"
"		const float i, const float i_bf,\n"
"#endif\n"
"		const float nuVal, const float nuVal_bf,\n"
"#if defined(PARAM_ENABLE_MAT_GLOSSYTRANSLUCENT_ANISOTROPIC)\n"
"		const float nvVal, const float nvVal_bf,\n"
"#endif\n"
"#if defined(PARAM_ENABLE_MAT_GLOSSYTRANSLUCENT_ABSORPTION)\n"
"		const float3 kaVal, const float3 kaVal_bf,\n"
"		const float d, const float d_bf,\n"
"#endif\n"
"#if defined(PARAM_ENABLE_MAT_GLOSSYTRANSLUCENT_MULTIBOUNCE)\n"
"		const int multibounceVal, const int multibounceVal_bf,\n"
"#endif\n"
"		const float3 kdVal, const float3 ktVal, const float3 ksVal, const float3 ksVal_bf) {\n"
"	if (fabs(fixedDir.z) < DEFAULT_COS_EPSILON_STATIC)\n"
"		return BLACK;\n"
"\n"
"	if (passThroughEvent < .5f) {\n"
"		// Reflection\n"
"		float3 ks;\n"
"#if defined(PARAM_ENABLE_MAT_GLOSSYTRANSLUCENT_INDEX)\n"
"		float index;\n"
"#endif\n"
"		float u;\n"
"#if defined(PARAM_ENABLE_MAT_GLOSSYTRANSLUCENT_ANISOTROPIC)\n"
"		float v;\n"
"#endif\n"
"#if defined(PARAM_ENABLE_MAT_GLOSSYTRANSLUCENT_ABSORPTION)\n"
"		float3 alpha;\n"
"		float depth;\n"
"#endif\n"
"#if defined(PARAM_ENABLE_MAT_GLOSSYTRANSLUCENT_MULTIBOUNCE)\n"
"		int mbounce;\n"
"#else\n"
"		int mbounce = 0;\n"
"#endif\n"
"		if (fixedDir.z >= 0.f) {\n"
"			ks = ksVal;\n"
"#if defined(PARAM_ENABLE_MAT_GLOSSYTRANSLUCENT_INDEX)\n"
"			index = i;\n"
"#endif\n"
"			u = clamp(nuVal, 1e-9f, 1.f);\n"
"#if defined(PARAM_ENABLE_MAT_GLOSSYTRANSLUCENT_ANISOTROPIC)\n"
"			v = clamp(nvVal, 1e-9f, 1.f);\n"
"#endif\n"
"#if defined(PARAM_ENABLE_MAT_GLOSSYTRANSLUCENT_ABSORPTION)\n"
"			alpha = Spectrum_Clamp(kaVal);\n"
"			depth = d;\n"
"#endif\n"
"#if defined(PARAM_ENABLE_MAT_GLOSSYTRANSLUCENT_MULTIBOUNCE)\n"
"			mbounce = multibounceVal;\n"
"#endif\n"
"		} else {\n"
"			ks = ksVal_bf;\n"
"#if defined(PARAM_ENABLE_MAT_GLOSSYTRANSLUCENT_INDEX)\n"
"			index = i_bf;\n"
"#endif\n"
"			u = clamp(nuVal_bf, 1e-9f, 1.f);\n"
"#if defined(PARAM_ENABLE_MAT_GLOSSYTRANSLUCENT_ANISOTROPIC)\n"
"			v = clamp(nvVal_bf, 1e-9f, 1.f);\n"
"#endif\n"
"#if defined(PARAM_ENABLE_MAT_GLOSSYTRANSLUCENT_ABSORPTION)\n"
"			alpha = Spectrum_Clamp(kaVal_bf);\n"
"			depth = d_bf;\n"
"#endif\n"
"#if defined(PARAM_ENABLE_MAT_GLOSSYTRANSLUCENT_MULTIBOUNCE)\n"
"			mbounce = multibounceVal_bf;\n"
"#endif\n"
"		}\n"
"\n"
"#if defined(PARAM_ENABLE_MAT_GLOSSYTRANSLUCENT_INDEX)\n"
"		if (index > 0.f) {\n"
"			const float ti = (index - 1.f) / (index + 1.f);\n"
"			ks *= ti * ti;\n"
"		}\n"
"#endif\n"
"		ks = Spectrum_Clamp(ks);\n"
"\n"
"#if defined(PARAM_ENABLE_MAT_GLOSSYTRANSLUCENT_ANISOTROPIC)\n"
"		const float u2 = u * u;\n"
"		const float v2 = v * v;\n"
"		const float anisotropy = (u2 < v2) ? (1.f - u2 / v2) : u2 > 0.f ? (v2 / u2 - 1.f) : 0.f;\n"
"		const float roughness = u * v;\n"
"#else\n"
"		const float anisotropy = 0.f;\n"
"		const float roughness = u * u;\n"
"#endif\n"
"\n"
"		const float wCoating = SchlickBSDF_CoatingWeight(ks, fixedDir);\n"
"		const float wBase = 1.f - wCoating;\n"
"\n"
"		float basePdf, coatingPdf;\n"
"		float3 baseF, coatingF;\n"
"\n"
"		if (2.f * passThroughEvent < wBase) {\n"
"			// Sample base BSDF (Matte BSDF)\n"
"			*sampledDir = CosineSampleHemisphereWithPdf(u0, u1, &basePdf);\n"
"			*sampledDir *= signbit(fixedDir.z) ? -1.f : 1.f;\n"
"\n"
"			*cosSampledDir = fabs((*sampledDir).z);\n"
"			if (*cosSampledDir < DEFAULT_COS_EPSILON_STATIC)\n"
"				return BLACK;\n"
"\n"
"			baseF = Spectrum_Clamp(kdVal) * M_1_PI_F * *cosSampledDir;\n"
"\n"
"			// Evaluate coating BSDF (Schlick BSDF)\n"
"			coatingF = SchlickBSDF_CoatingF(ks, roughness, anisotropy, mbounce,\n"
"				fixedDir, *sampledDir);\n"
"			coatingPdf = SchlickBSDF_CoatingPdf(roughness, anisotropy, fixedDir, *sampledDir);\n"
"\n"
"			*event = GLOSSY | REFLECT;\n"
"		} else {\n"
"			// Sample coating BSDF (Schlick BSDF)\n"
"			coatingF = SchlickBSDF_CoatingSampleF(ks, roughness, anisotropy, mbounce,\n"
"				fixedDir, sampledDir, u0, u1, &coatingPdf);\n"
"			if (Spectrum_IsBlack(coatingF))\n"
"				return BLACK;\n"
"\n"
"			*cosSampledDir = fabs((*sampledDir).z);\n"
"			if (*cosSampledDir < DEFAULT_COS_EPSILON_STATIC)\n"
"				return BLACK;\n"
"\n"
"			coatingF *= coatingPdf;\n"
"\n"
"			// Evaluate base BSDF (Matte BSDF)\n"
"			basePdf = *cosSampledDir * M_1_PI_F;\n"
"			baseF = Spectrum_Clamp(kdVal) * M_1_PI_F * *cosSampledDir;\n"
"\n"
"			*event = GLOSSY | REFLECT;\n"
"		}\n"
"\n"
"		// Note: this is the same side test used by matte translucent material and\n"
"		// it is different from the CPU test because HitPoint::dpdu and HitPoint::dpdv\n"
"		// are not available here without bump mapping.\n"
"		const float sideTest = CosTheta(fixedDir) * CosTheta(*sampledDir);\n"
"		if (!(sideTest > DEFAULT_COS_EPSILON_STATIC))\n"
"			return BLACK;\n"
"\n"
"		*pdfW = .5f * (coatingPdf * wCoating + basePdf * wBase);\n"
"\n"
"		// Absorption\n"
"#if defined(PARAM_ENABLE_MAT_GLOSSYTRANSLUCENT_ABSORPTION)\n"
"		const float cosi = fabs((*sampledDir).z);\n"
"		const float coso = fabs(fixedDir.z);\n"
"		const float3 absorption = CoatingAbsorption(cosi, coso, alpha, d);\n"
"#else\n"
"		const float3 absorption = WHITE;\n"
"#endif\n"
"\n"
"		// Coating fresnel factor\n"
"		const float3 H = normalize(fixedDir + *sampledDir);\n"
"		const float3 S = FresnelSchlick_Evaluate(ks, fabs(dot(*sampledDir, H)));\n"
"\n"
"		// Blend in base layer Schlick style\n"
"		// coatingF already takes fresnel factor S into account\n"
"\n"
"		return (coatingF + absorption * (WHITE - S) * baseF) / *pdfW;\n"
"	} else {\n"
"		// Transmission\n"
"		*sampledDir = CosineSampleHemisphereWithPdf(u0, u1, pdfW);\n"
"		*sampledDir *= signbit(fixedDir.z) ? 1.f : -1.f;\n"
"\n"
"		// Note: this is the same side test used by matte translucent material and\n"
"		// it is different from the CPU test because HitPoint::dpdu and HitPoint::dpdv\n"
"		// are not available here without bump mapping.\n"
"		const float sideTest = CosTheta(fixedDir) * CosTheta(*sampledDir);\n"
"		if (!(sideTest < -DEFAULT_COS_EPSILON_STATIC))\n"
"			return BLACK;\n"
"\n"
"		*pdfW *= .5f;\n"
"\n"
"		*cosSampledDir = fabs((*sampledDir).z);\n"
"		if (*cosSampledDir < DEFAULT_COS_EPSILON_STATIC)\n"
"			return BLACK;\n"
"\n"
"		*event = DIFFUSE | TRANSMIT;\n"
"\n"
"		const float cosi = fabs((*sampledDir).z);\n"
"		const float coso = fabs(fixedDir.z);\n"
"\n"
"		const float3 H = normalize((float3)((*sampledDir).x + fixedDir.x, (*sampledDir).y + fixedDir.y,\n"
"			(*sampledDir).z - fixedDir.z));\n"
"		const float u = fabs(dot(*sampledDir, H));\n"
"		float3 ks = ksVal;\n"
"#if defined(PARAM_ENABLE_MAT_GLOSSYTRANSLUCENT_INDEX)\n"
"		if (i > 0.f) {\n"
"			const float ti = (i - 1.f) / (i + 1.f);\n"
"			ks *= ti * ti;\n"
"		}\n"
"#endif\n"
"		ks = Spectrum_Clamp(ks);\n"
"		const float3 S1 = FresnelSchlick_Evaluate(ks, u);\n"
"\n"
"		ks = ksVal_bf;\n"
"#if defined(PARAM_ENABLE_MAT_GLOSSYTRANSLUCENT_INDEX)\n"
"		if (i_bf > 0.f) {\n"
"			const float ti = (i_bf - 1.f) / (i_bf + 1.f);\n"
"			ks *= ti * ti;\n"
"		}\n"
"#endif\n"
"		ks = Spectrum_Clamp(ks);\n"
"		const float3 S2 = FresnelSchlick_Evaluate(ks, u);\n"
"		float3 S = Spectrum_Sqrt((WHITE - S1) * (WHITE - S2));\n"
"#if defined(PARAM_ENABLE_MAT_GLOSSYTRANSLUCENT_ABSORPTION)\n"
"		if ((*sampledDir).z > 0.f) {\n"
"			S *= Spectrum_Exp(Spectrum_Clamp(kaVal) * -(d / cosi) +\n"
"				Spectrum_Clamp(kaVal_bf) * -(d_bf / coso));\n"
"		} else {\n"
"			S *= Spectrum_Exp(Spectrum_Clamp(kaVal) * -(d / coso) +\n"
"				Spectrum_Clamp(kaVal_bf) * -(d_bf / cosi));\n"
"		}\n"
"#endif\n"
"		return (M_1_PI_F * cosi / *pdfW) * S * Spectrum_Clamp(ktVal) *\n"
"			(WHITE - Spectrum_Clamp(kdVal));\n"
"	}\n"
"}\n"
"\n"
"#endif\n"
; } }
