////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Device
//
// Copyright (C) Microsoft Corporation
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "d3dumddi_.h"
#include <debugapi.h>

#include "RosUmdCommandBuffer.h"
#include "RosAllocation.h"
#include "RosUmdUtil.h"
#include "RosUmdDebug.h"

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif

class RosUmdAdapter;
class RosUmdRenderTargetView;
class RosUmdBlendState;
class RosUmdShader;
class RosUmdElementLayout;
class RosUmdDepthStencilState;
class RosUmdRasterizerState;
class RosUmdSampler;

//==================================================================================================================================
//
// RosUmdDevice
//
//==================================================================================================================================
class RosUmdDevice 
{
public:
    explicit RosUmdDevice( RosUmdAdapter*, const D3D10DDIARG_CREATEDEVICE* );
    ~RosUmdDevice();

    void Standup();
    void Teardown();

    static RosUmdDevice* CastFrom( D3D10DDI_HDEVICE );
    D3D10DDI_HDEVICE CastTo() const;

public:

    void CreateResource(const D3D11DDIARG_CREATERESOURCE* pCreateResource, D3D10DDI_HRESOURCE hResource, D3D10DDI_HRTRESOURCE hRTResource);
    void DestroyResource(RosUmdResource * pResource);
    void ResourceCopy(RosUmdResource *pDestinationResource, RosUmdResource * pSourceResource);

    void StagingResourceMap(RosUmdResource * pResource, UINT subResource, D3D10_DDI_MAP mapType, UINT mapFlags, D3D10DDI_MAPPED_SUBRESOURCE* pMappedSubRes);
    void StagingResourceUnmap(RosUmdResource * pResource, UINT subResource);

public:

    void CheckFormatSupport(DXGI_FORMAT inFormat, UINT* pOutFormatSupport);
    void CheckCounterInfo(D3D10DDI_COUNTER_INFO* pOutCounterInfo);
    void CheckMultisampleQualityLevels(DXGI_FORMAT inFormat, UINT inSampleCount, UINT* pOutNumQualityLevels);

public:

    //
    // Kernel call backs
    //

    void Allocate(D3DDDICB_ALLOCATE * pAllocate);
    void Lock(D3DDDICB_LOCK * pLock);
    void Unlock(D3DDDICB_UNLOCK * pLock);
    void Render(D3DDDICB_RENDER * pRender);
    void DestroyContext(D3DDDICB_DESTROYCONTEXT * pDestroyContext);

    //
    // User mode call backs
    //

    void SetError(HRESULT hr);

public:

    void SetException(std::exception & e);
    void SetException(RosUmdException & e);

public:
    HANDLE                          m_hContext;

    RosUmdAdapter*                  m_pAdapter;
    UINT                            m_Interface;
    D3D11DDI_3DPIPELINELEVEL        m_PipelineLevel;

    // Pointer to function table that contains the callbacks to the runtime. Runtime is free to change pointers, so do not cache
    // function pointers themselves.
    const D3DDDI_DEVICECALLBACKS*   m_pMSKTCallbacks;

    const D3D11DDI_CORELAYER_DEVICECALLBACKS*   m_pMSUMCallbacks;
    const DXGI_DDI_BASE_CALLBACKS*              m_pDXGICallbacks;

    // Handle for runtime device to use with runtime callbacks.
    D3D10DDI_HRTDEVICE              m_hRTDevice;
    D3D10DDI_HRTCORELAYER           m_hRTCoreLayer;

    // Pointer to function table that runtime will use. Driver is free to change function pointers while the driver has context
    // within one of these entry points.
    union
    {
        D3D10DDI_DEVICEFUNCS*       m_pDeviceFuncs;
        D3D10_1DDI_DEVICEFUNCS*     m_p10_1DeviceFuncs;
        D3D11DDI_DEVICEFUNCS*       m_p11DeviceFuncs;
        D3D11_1DDI_DEVICEFUNCS*     m_p11_1DeviceFuncs;
        D3DWDDM1_3DDI_DEVICEFUNCS*  m_pWDDM1_3DeviceFuncs;
        D3DWDDM2_0DDI_DEVICEFUNCS*  m_pWDDM2_0DeviceFuncs;
    };

    RosUmdCommandBuffer             m_commandBuffer;

public:

    //
    // Draw Support
    //

    void Draw(UINT VertexCount, UINT StartVertexLocation);
    void ClearRenderTargetView(RosUmdRenderTargetView * pRenderTargetView, FLOAT clearColor[4]);

public:

    //
    // Graphics State Management
    //
    static const UINT kMaxVertexBuffers = D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT;
    static const UINT kMaxViewports = D3D10_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
    static const UINT kMaxRenderTargets = D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT;
    static const UINT kMaxSamplers = D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT;

    void SetVertexBuffers(UINT startBuffer, UINT numBuffers, const D3D10DDI_HRESOURCE* phBuffers, const UINT* pStrides,  const UINT* pOffsets);
    void SetTopology(D3D10_DDI_PRIMITIVE_TOPOLOGY topology);
    void SetViewports(UINT numViewports, UINT clearViewports, const D3D10_DDI_VIEWPORT* pViewports);
    void SetRenderTargets(const D3D10DDI_HRENDERTARGETVIEW* phRenderTargetView, UINT NumRTVs, UINT RTVNumbertoUnbind,
        D3D10DDI_HDEPTHSTENCILVIEW hDepthStencilView, const D3D11DDI_HUNORDEREDACCESSVIEW* phUnorderedAccessView,
        const UINT* pUAVInitialCounts, UINT UAVIndex, UINT NumUAVs, UINT UAVFirsttoSet, UINT UAVNumberUpdated);
    void SetBlendState(RosUmdBlendState * pBlendState, const FLOAT pBlendFactor[4], UINT sampleMask);
    void SetPixelShader(RosUmdShader * pShader);
    void SetPixelSamplers(UINT Offset, UINT NumSamplers, const D3D10DDI_HSAMPLER* phSamplers);
    void SetVertexShader(RosUmdShader * pShader);
    void SetVertexSamplers(UINT Offset, UINT NumSamplers, const D3D10DDI_HSAMPLER* phSamplers);
    void SetDomainShader(RosUmdShader * pShader);
    void SetDomainSamplers(UINT Offset, UINT NumSamplers, const D3D10DDI_HSAMPLER* phSamplers);
    void SetGeometryShader(RosUmdShader * pShader);
    void SetGeometrySamplers(UINT Offset, UINT NumSamplers, const D3D10DDI_HSAMPLER* phSamplers);
    void SetHullShader(RosUmdShader * pShader);
    void SetHullSamplers(UINT Offset, UINT NumSamplers, const D3D10DDI_HSAMPLER* phSamplers);
    void SetComputeShader(RosUmdShader * pShader);
    void SetComputeSamplers(UINT Offset, UINT NumSamplers, const D3D10DDI_HSAMPLER* phSamplers);
    void SetElementLayout(RosUmdElementLayout * pElementLayout);
    void SetDepthStencilState(RosUmdDepthStencilState * pDepthStencilState, UINT stencilRef);
    void SetRasterizerState(RosUmdRasterizerState * pRasterizerState);


    RosUmdResource *                m_vertexBuffers[kMaxVertexBuffers];
    UINT                            m_vertexStrides[kMaxVertexBuffers];
    UINT                            m_vertexOffsets[kMaxVertexBuffers];
    UINT                            m_numVertexBuffers;

    D3D10_DDI_PRIMITIVE_TOPOLOGY    m_topology;

    D3D10_DDI_VIEWPORT              m_viewports[kMaxViewports];
    UINT                            m_numViewports;

    RosUmdRenderTargetView *        m_renderTargetViews[kMaxRenderTargets];
    UINT                            m_numRenderTargetViews;

    RosUmdBlendState *              m_blendState;
    FLOAT                           m_blendFactor[4];
    UINT                            m_sampleMask;

    RosUmdShader *                  m_pixelShader;
    RosUmdSampler *                 m_pixelSamplers[kMaxSamplers];

    RosUmdShader *                  m_vertexShader;
    RosUmdSampler *                 m_vertexSamplers[kMaxSamplers];

    RosUmdShader *                  m_domainShader;
    RosUmdSampler *                 m_domainSamplers[kMaxSamplers];

    RosUmdShader *                  m_geometryShader;
    RosUmdSampler *                 m_geometrySamplers[kMaxSamplers];

    RosUmdShader *                  m_hullShader;
    RosUmdSampler *                 m_hullSamplers[kMaxSamplers];

    RosUmdShader *                  m_computeShader;
    RosUmdSampler *                 m_computeSamplers[kMaxSamplers];

    RosUmdElementLayout *           m_elementLayout;

    RosUmdDepthStencilState *       m_depthStencilState;
    UINT                            m_stencilRef;

    RosUmdRasterizerState *         m_rasterizerState;


};

inline RosUmdDevice* RosUmdDevice::CastFrom(D3D10DDI_HDEVICE hDevice)
{
    return static_cast< RosUmdDevice* >(hDevice.pDrvPrivate);
}

inline D3D10DDI_HDEVICE RosUmdDevice::CastTo() const
{
    return MAKE_D3D10DDI_HDEVICE(const_cast< RosUmdDevice* >(this));
}
