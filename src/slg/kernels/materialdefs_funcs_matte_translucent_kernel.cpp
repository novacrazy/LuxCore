#include <string>
namespace slg { namespace ocl {
std::string KernelSource_materialdefs_funcs_matte_translucent = 
"#line 2 \"materialdefs_funcs_matte_translucent.cl\"\n"
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
"// MatteTranslucent material\n"
"//------------------------------------------------------------------------------\n"
"\n"
"#if defined (PARAM_ENABLE_MAT_MATTETRANSLUCENT)\n"
"\n"
"BSDFEvent MatteTranslucentMaterial_GetEventTypes() {\n"
"	return DIFFUSE | REFLECT | TRANSMIT;\n"
"}\n"
"\n"
"float3 MatteTranslucentMaterial_Evaluate(\n"
"		__global HitPoint *hitPoint, const float3 lightDir, const float3 eyeDir,\n"
"		BSDFEvent *event, float *directPdfW,\n"
"		const float3 krVal, const float3 ktVal) {\n"
"	const float3 r = Spectrum_Clamp(krVal);\n"
"	const float3 t = Spectrum_Clamp(ktVal) * \n"
"		// Energy conservation\n"
"		(1.f - r);\n"
"\n"
"	const bool isKtBlack = Spectrum_IsBlack(t);\n"
"	const bool isKrBlack = Spectrum_IsBlack(r);\n"
"\n"
"	// Decide to transmit or reflect\n"
"	float threshold;\n"
"	if (!isKrBlack) {\n"
"		if (!isKtBlack)\n"
"			threshold = .5f;\n"
"		else\n"
"			threshold = 1.f;\n"
"	} else {\n"
"		if (!isKtBlack)\n"
"			threshold = 0.f;\n"
"		else {\n"
"			if (directPdfW)\n"
"				*directPdfW = 0.f;\n"
"			return BLACK;\n"
"		}\n"
"	}\n"
"\n"
"	const bool relfected = (CosTheta(lightDir) * CosTheta(eyeDir) > 0.f);\n"
"	const float weight = (lightDir.z * eyeDir.z > 0.f) ? threshold : (1.f - threshold);\n"
"\n"
"	if (directPdfW)\n"
"		*directPdfW = weight * fabs(lightDir.z * M_1_PI_F);\n"
"\n"
"	if (lightDir.z * eyeDir.z > 0.f) {\n"
"		*event = DIFFUSE | REFLECT;\n"
"		return r * fabs(lightDir.z * M_1_PI_F);\n"
"	} else {\n"
"		*event = DIFFUSE | TRANSMIT;\n"
"		return t * fabs(lightDir.z * M_1_PI_F);\n"
"	}\n"
"}\n"
"\n"
"float3 MatteTranslucentMaterial_Sample(\n"
"		__global HitPoint *hitPoint, const float3 fixedDir, float3 *sampledDir,\n"
"		const float u0, const float u1,\n"
"#if defined(PARAM_HAS_PASSTHROUGH)\n"
"		const float passThroughEvent,\n"
"#endif\n"
"		float *pdfW, float *cosSampledDir, BSDFEvent *event,\n"
"		const float3 krVal, const float3 ktVal) {\n"
"	if (fabs(fixedDir.z) < DEFAULT_COS_EPSILON_STATIC)\n"
"		return BLACK;\n"
"\n"
"	*sampledDir = CosineSampleHemisphereWithPdf(u0, u1, pdfW);\n"
"	*cosSampledDir = fabs((*sampledDir).z);\n"
"	if (*cosSampledDir < DEFAULT_COS_EPSILON_STATIC)\n"
"		return BLACK;\n"
"\n"
"	const float3 kr = Spectrum_Clamp(krVal);\n"
"	const float3 kt = Spectrum_Clamp(ktVal) * \n"
"		// Energy conservation\n"
"		(1.f - kr);\n"
"\n"
"	const bool isKtBlack = Spectrum_IsBlack(kt);\n"
"	const bool isKrBlack = Spectrum_IsBlack(kr);\n"
"	if (isKtBlack && isKrBlack)\n"
"		return BLACK;\n"
"\n"
"	// Decide to transmit or reflect\n"
"	float threshold;\n"
"	if ((requestedEvent & REFLECT) && !isKrBlack) {\n"
"		if ((requestedEvent & TRANSMIT) && !isKtBlack)\n"
"			threshold = .5f;\n"
"		else\n"
"			threshold = 1.f;\n"
"	} else {\n"
"		if ((requestedEvent & TRANSMIT) && !isKtBlack)\n"
"			threshold = 0.f;\n"
"		else\n"
"			return BLACK;\n"
"	}\n"
"\n"
"	if (passThroughEvent < threshold) {\n"
"		*sampledDir *= (signbit(fixedDir.z) ? -1.f : 1.f);\n"
"		*event = DIFFUSE | REFLECT;\n"
"		*pdfW *= threshold;\n"
"\n"
"		return kr / threshold;\n"
"	} else {\n"
"		*sampledDir *= -(signbit(fixedDir.z) ? -1.f : 1.f);\n"
"		*event = DIFFUSE | TRANSMIT;\n"
"		*pdfW *= (1.f - threshold);\n"
"\n"
"		return kt / (1.f - threshold);\n"
"	}\n"
"}\n"
"\n"
"#endif\n"
; } }
