#if 0

	#define _SAL_VERSION 20
	#include <sal.h>

	
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#ifndef SAL_STUB_H
#define SAL_STUB_H

	#ifndef _In_
	#define _In_
	#endif

	#ifndef _Out_
	#define _Out_
	#endif

	#ifndef _In_opt_
	#define _In_opt_
	#endif

	#ifndef _Outptr_
	#define _Outptr_
	#endif

	#ifndef _Maybenull_
	#define _Maybenull_
	#endif

#endif


#include <DirectML/directx/d3d12.h>
//#include <DirectML/directx/dxgi1_6.h>
#include <DirectML/directx/DirectML.h>
#include <iostream>
#include <vector>
#include <cmath>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

bool nearlyEqual(float a, float b, float eps = 1e-4f) {
    return std::fabs(a - b) < eps;
}

int main() {
    IDXGIFactory6* factory = nullptr;
    CreateDXGIFactory1(IID_PPV_ARGS(&factory));

    IDXGIAdapter1* adapter = nullptr;
    for (UINT i = 0; factory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND; i++) {
        DXGI_ADAPTER_DESC1 desc;
        adapter->GetDesc1(&desc);
        if (!(desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE))
            break;
    }

    ID3D12Device* d3d12Device = nullptr;
    D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&d3d12Device));

    IDMLDevice* dmlDevice = nullptr;
    DMLCreateDevice(d3d12Device, DML_CREATE_DEVICE_FLAG_NONE, IID_PPV_ARGS(&dmlDevice));

    ID3D12CommandQueue* queue = nullptr;
    D3D12_COMMAND_QUEUE_DESC qdesc = {};
    qdesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    d3d12Device->CreateCommandQueue(&qdesc, IID_PPV_ARGS(&queue));

    ID3D12CommandAllocator* allocator = nullptr;
    d3d12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocator));

    ID3D12GraphicsCommandList* cmdList = nullptr;
    d3d12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, allocator, nullptr, IID_PPV_ARGS(&cmdList));

    IDMLCommandRecorder* recorder = nullptr;
    dmlDevice->CreateCommandRecorder(IID_PPV_ARGS(&recorder));

    // Tensorbeschreibungen
    uint32_t dimsA[2] = {2, 3};
    uint32_t dimsB[2] = {3, 2};
    uint32_t dimsOut[2] = {2, 2};

    DML_BUFFER_TENSOR_DESC bufA = {DML_TENSOR_DATA_TYPE_FLOAT32, DML_TENSOR_FLAG_NONE, 2, dimsA, nullptr, sizeof(float)*6};
    DML_BUFFER_TENSOR_DESC bufB = {DML_TENSOR_DATA_TYPE_FLOAT32, DML_TENSOR_FLAG_NONE, 2, dimsB, nullptr, sizeof(float)*6};
    DML_BUFFER_TENSOR_DESC bufOut = {DML_TENSOR_DATA_TYPE_FLOAT32, DML_TENSOR_FLAG_NONE, 2, dimsOut, nullptr, sizeof(float)*4};

    DML_TENSOR_DESC tA = {DML_TENSOR_TYPE_BUFFER, &bufA};
    DML_TENSOR_DESC tB = {DML_TENSOR_TYPE_BUFFER, &bufB};
    DML_TENSOR_DESC tOut = {DML_TENSOR_TYPE_BUFFER, &bufOut};

    DML_MATRIX_MULTIPLY_OPERATOR_DESC matmulDesc = {&tA, &tB, &tOut};
    DML_OPERATOR_DESC opDesc = {DML_OPERATOR_MATRIX_MULTIPLY, &matmulDesc};

    IDMLCompiledOperator* compiledOp = nullptr;
    dmlDevice->CompileOperator(&opDesc, DML_EXECUTION_FLAG_NONE, IID_PPV_ARGS(&compiledOp));

    auto createBuffer = [&](size_t size) {
        ID3D12Resource* res = nullptr;
        D3D12_HEAP_PROPERTIES props = {D3D12_HEAP_TYPE_UPLOAD};
        D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(size);
        d3d12Device->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &desc,
                                             D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                                             IID_PPV_ARGS(&res));
        return res;
    };

    ID3D12Resource* bufARes = createBuffer(sizeof(float)*6);
    ID3D12Resource* bufBRes = createBuffer(sizeof(float)*6);
    ID3D12Resource* bufOutRes = createBuffer(sizeof(float)*4);

    float A[6] = {1,2,3,4,5,6};
    float B[6] = {7,8,9,10,11,12};

    void* ptr;
    bufARes->Map(0, nullptr, &ptr); memcpy(ptr, A, sizeof(A)); bufARes->Unmap(0, nullptr);
    bufBRes->Map(0, nullptr, &ptr); memcpy(ptr, B, sizeof(B)); bufBRes->Unmap(0, nullptr);

    IDMLBindingTable* table = nullptr;
    DML_BINDING_TABLE_DESC tableDesc = {compiledOp, {}, {}, 0};
    dmlDevice->CreateBindingTable(&tableDesc, IID_PPV_ARGS(&table));

    DML_BUFFER_BINDING bindA = {bufARes, 0, sizeof(A)};
    DML_BUFFER_BINDING bindB = {bufBRes, 0, sizeof(B)};
    DML_BUFFER_BINDING bindOut = {bufOutRes, 0, sizeof(float)*4};

    DML_BINDING_DESC inputs[2] = {
        {DML_BINDING_TYPE_BUFFER, &bindA},
        {DML_BINDING_TYPE_BUFFER, &bindB}
    };
    DML_BINDING_DESC output = {DML_BINDING_TYPE_BUFFER, &bindOut};

    table->BindInputs(2, inputs);
    table->BindOutputs(1, &output);

    recorder->RecordDispatch(cmdList, compiledOp, table);
    cmdList->Close();

    ID3D12CommandList* lists[] = {cmdList};
    queue->ExecuteCommandLists(1, lists);

    ID3D12Fence* fence = nullptr;
    d3d12Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
    HANDLE evt = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    queue->Signal(fence, 1);
    fence->SetEventOnCompletion(1, evt);
    WaitForSingleObject(evt, INFINITE);

    float out[4];
    bufOutRes->Map(0, nullptr, &ptr);
    memcpy(out, ptr, sizeof(out));
    bufOutRes->Unmap(0, nullptr);

    float expected[4] = {58, 64, 139, 154};

    bool ok = true;
    for (int i = 0; i < 4; i++) {
        if (!nearlyEqual(out[i], expected[i])) ok = false;
    }

    if (ok) {
        std::cout << "DirectML OK – Ergebnis korrekt!\n";
    } else {
        std::cout << "FEHLER: Ergebnis falsch!\n";
        std::cout << "Erhalten: ";
        for (float v : out) std::cout << v << " ";
        std::cout << "\nErwartet: ";
        for (float v : expected) std::cout << v << " ";
        std::cout << "\n";
    }

    return 0;
}
