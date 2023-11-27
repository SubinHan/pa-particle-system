## Particle System

### ����
[������Ʈ ����](#������Ʈ-����) <br>
[��ǥ �� ��ȹ](#��ǥ-��-��ȹ) <br>
[Tasks](#Tasks) <br>
[����](#����) <br>
[���� ���� ����](#����-����-����) <br>
[������](#������)

<hr/>

### ������Ʈ ����

�ܼ��� ������ ��ƼŬ �ý����� D3D12�� �����մϴ�. <br>
Ư��, **Simulation�� �ƴ� Particle Control�� ����**�մϴ�. <br>


#### ���� ��ǥ Reference
* ***Unreal Engine 5 Niagara*** <br>
* [YouTube: ncParticleEditor](https://www.youtube.com/watch?v=RLNI5NMCJ1E) <br>
* [YouTube: OpenGL/Imgui Engine Build Stage 3.5: Particle System](https://www.youtube.com/watch?v=rZ3ztv7u0Yk) <br>
* [YouTube: 2D Game Particle tool using IMGUI in DirectX11](https://www.youtube.com/watch?v=TXf4D8Ess6s) <br>
* [YouTube: Compute Shader Particle System pt. 3, GUI](https://www.youtube.com/watch?v=UqB4paFZcBg) <br>
* [YouTube: Custom Particle Editor](https://www.youtube.com/watch?v=iQhOHd_JSgk) <br>

<hr/>

### ��ǥ �� ��ȹ

##### 1����: ��ȹ ���� �� ���� ȯ�� ����
* ���� ���� �� ����
* D3D12 ���� ȯ�� ����


##### 2~4����: �ٽ� ��� ���� �� Particle System �н�

* �н�
* D3D12 ������ ������ ����
* emnitter, particle �� �ٽ� ��� ���� �� ����
* Computing Shader �ۼ�
  + EmitterCS �ۼ�
  + SimulateCS �ۼ�
* Particle Rendering ����
  + VS, GS, PS �ۼ�
* �ܼ��� ������ particle simulation ���� (i.e. sprinkler)
* z-buffer�� ���� particle occlusion
* *motion blur ���� (���� ��Ȳ�� ���� ����)*
* *alpha-blending (���� ��Ȳ�� ���� ����)*
  + *SortingCS �ۼ�*
  + *alpha-blending ����*
* *geometry���� collision (���� ��Ȳ�� ���� ����)*

##### 5~7����: UI ���� �� ��뼺, Ȯ�强, ���������� ����

* imgui ����
* hard coding �Ǿ� ���� �پ��� ���� parameterȭ
* ������ particle control�� ����, �ٽ� ����� ���� ����
* �پ��� particle contorl�� ����, ��� ���� (i.e. gravity, drag, noise, wind, ...)


##### 8����: ������
* refactoring
* visuality ����

#### ���� ���
* Native�� D3D12�� ���� (Mini Engine �� ��� X)
* Particle System �н� �̺�
* imgui ��� ���� ����

<hr/>

### Tasks

##### Backlogs
* Mesh(i.e. cube) render
* Particle�� Orientation �Ӽ� �߰�
* Opaque particles�� ���� draw�ؾ� ��
* Angular Velocity, Orientation ����
* ������ ��� ���
* Motion blur
* Collision
* Lighting
* Curl Noise�� Perlin Noise �������
* Shader generation���� node dependency ���Ἲ Ȯ��
  * i.e.) float3�� float4�� ���������� �ʴ���?
  * �� node���� type ������ �����Ͽ� ������ input���� �����ؾ� ��
* ���� �̽�:
  *

##### ���� ���� Task
* ���� ����ȭ
* sprite rendering �� quad�� �ƴ� bounding n-gon���� geometry generation

##### �Ϸ�� Tasks
* D3D12 ���� ȯ�� ���� (+PIX �����)
* D3D12 ������ ������ ����
* ������ ������ Particle ����/�ı�, �ùķ��̼��� ���� Computing Shader �ۼ�
* ������ ������ Particle System�� ���� ������ ���������� ����
* ������ ������ emitter, particle �� �ٽ� ��� ���� �� ����
* Texture mapping ����
* alpha blending ����
* Sorting ���� Ȯ��
  + Batcher's odd-even merge sort �н�
  + Bitonic sort �н�
  + ������ ���� ���� �� ��� Ȯ��
* HLSL Generator ����
  * �߻�ȭ�� �۾����� HLSL �ڵ�� ��ȯ�����ִ� Ŭ����
  * ���� float4 ������ �ʱ�ȭ�ϰ�, �� ���� float4 ������ ���ϴ� �� ��
  * �̸� ������� ������ Emit, Simulate process ������ ��� ����
* DrawIndexedIndirect(), ExecuteIndirect() �н� �� ����
* UI:
  * Spawn rate ���� ����
  * Emitter Shader �ۼ� ����
  * Simulator Shader �ۼ� ����
  * Renderer Shader �ۼ� ����
  * ���ο� Particle System �߰� ����
  * Particle System�� ���¸� ���� �� �ε�
  * Opaque/Translucent ��ȯ ����
  * input node�� ��ȿ���� �ʴٸ� ���������� ����
* Simulation Node:
  * Vortex Force
  * Point Attraction Force
  * Drag Force
  * Curl Noise Force
* �� ������
* Orphan node���� shader code�� ��ȯ���� ����
* Ribbon ������
* ���� ����ȭ
  * ExecuteIndirect���� ������ maxCommandsCount�� �ѱ�� (���� ���� - 4���� �����)
  * Instancing�� �̿��� ExecuteIndirect�� commands ������ ���̱�
  * Caching�� ����� ��ƼŬ ���� ����
  * ��ƼŬ ũ�⸦ ���̱� ���� data packing/unpacking
  * 

<hr/>

### ����
 
##### 1����: ��ȹ ���� �� ���� ȯ�� ���� (2023.10.10. ~ 2023.10.13.)
* ������:
* �����:
  + D3D12 ȯ�� ����
  + ������ ���������� ����
* �ݿ���: 
  + PIX ����� ���� �� ȯ�� ����
  + ������ü ������ �Ϸ�
  + �ǻ� ���� (https://stackoverflow.com/questions/4200224/random-noise-functions-for-glsl)
  + Emitter, Particle �ڷᱸ�� prototype
  + ParticleEmitterCS, ParticleSimulateCS prototype

<br> 

##### 2����: �ٽ� ��� ���� �� Particle System �н� (2023.10.16. ~ 2023.10.20.)
* ������:
  + ParticleEmitterCS, ParticleSimulateCS �ۼ�
  + Particle �������� ���� ���̴� �Ϻ� �ۼ�
  + ParticleSimulateCS ����: RWByteAddressBuffer�� InterlockedAdd, Store, Load ��� �ڵ忡 ������ �־� ����ȭ�� ����� ������� ���� - �ذ�: ���̴� �ڵ尡 �ƴ� CPU �ڵ忡 ������ �־���. �޸� �ּҸ� �߸� �Ѱ��־� ���������� ���� ���� �������� �־���.
* ȭ����:
  + ������ RWByteAddressBuffer�� ������ ������ �־���: �ܼ��� UnorderedAccessBuffer�μ� RootSiganture���� �ٷ��������, RawBuffer�� �ݵ�� UnorderedAccessView�� ���� DXGI_FORMAT_R32_TYPELESS Ÿ�԰� D3D12_BUFFER_UAV_FLAG_RAW �÷��׸� ������ �ٷ��� ��! ������ ������ �̸� �������� �ʾҰ� ������ �߻��Ͽ��� ���̾���.
  + �⺻���� ��ƼŬ ������ ���� �Ϸ� (100�� ��)
    <img src="./img/20231017_particles.png">
  + ������ �ִ� ��ƼŬ ������ ���� �� �������� �ʵ��� ����
* ������:
  + Index Buffer Ÿ���� 16bit uint�� 2^16 �̻��� ��ƼŬ�� �ٷ�� ������ �߻��Ͽ� �ذ�.
  + Particle�� ����� �׻� Camera�� �ٶ󺸵��� ��
  + ParticleSystem Ŭ���� ���� �� ����
  + ���� ���� ParticleSystem�� ������ �� ���� (������ ������ particle system�� ���� �ٸ� world transform�� ���� �� ����)
  + ������ �ؽ�ó ���� (����: �뷫 10�� ��)
    <img src="./img/20231018_particles.png">
* �����:
  + ���� ���� ����
    <img src="./img/20231019_particles_alphablending.png">
  + Spawn rate ���� ����
  + Batcher's odd-even merge sort �н�
  + Bitonic sort �н�
  + Bitonic sort prototype (�ܼ��� �ε����� �������� �����غ�����)
* �ݿ���:
  * �������� ���� ���� utils�� �ڵ�ȭ ���� �׽�Ʈ ȯ�� ����
  * HlslTranslator (ParticleEmitter���� ���������� ����)
    * Graph �ڷᱸ�� ������� hlsl�� �����ϴ� Ŭ����
    * base shader ������ ������� Ư�� ������ �������� �ڵ� ���� �� ������
  * Object hashing
    * �� particle system ������ shader ������ ���� object ���ؿ����� hashing ����
    * ��ü ���� ������ �̿��� hashing 
  * DrawIndexedIndirect(), ExecuteIndirect() �н�

<br>

##### 3����: ���� ���̴� ������ ���� �� UI ���� (2023.10.23. ~ 2023.10.27.)
* ������:
  + Indirect Drawing ���� (�������� ������ �ִ� ��ƼŬ ����ŭ draw call ����)
  + ���� ���̴� ������ ����
    + Emit, Simulate, Render �ܰ迡���� ������ �������� ���� ����
  + Imgui �н� �� ���� ����
* ȭ����:
  * imgui ���� (+node graph ���� library�� imnodes)
  * Emitter�� ������ node editor ����
    * node editor ������� ���̴� ���� ����
* ������:
  * UI ����
    * Particle Emission, Simulation, Rendering ���� ����
    * ��� �������� �������� �����
      <img src="./img/20231025_ui.png">
* �����:
  * UI ����
    * Particle System���� ���¸� ������ �� ����
      * �ٽ� ���α׷� ���� �� �ڵ������� load
* �ݿ���:
  * Simulation Node �߰�
    * Drag Force
    * Vortex Force
    * Point Attraction Force
  * Simulation ��� ����
    * Velocity, Acceleration, Position�� ������ �����ϰ� �������� Solving ��
  * UI ����
    * Node�� ������ �� ����
    * �� Emitter, Simulator, Renderer Editor���� ����� �� �ִ� node�鸸 ������ �� ����
  * <img src="./img/20231027_particles.webp">
  * ��� ����(�ϴû� -> ������)


##### 4����: ���� Simulation �� ���� �� Fire ������ (2023.10.30. ~ 2023.11.03.)
* ������:
  + Perlin Noise �н�
  + Curl Noise�� Ȱ���ϱ� ���� ������ ����
    + �� Pass(Emitter, Simulator, Renderer)�� �������� ���ҽ�(�ؽ�ó)�� Ȱ���� �� ����.
    + � Shader statement nodes�� �����Ǿ����Ŀ� ���� �������� root signature, pso, shader�� ����
* ȭ����:
  * Curl Noise ����
  * �ڵ� �����丵 �� ������ ����
  * <img src="./img/curl_noise.webp">
  * ������ ��ƼŬ���� �ʱ⿡ burst �Ǵ� ���� �ǵ��� ���� �ƴϸ�, ����Ǵ� deltaTime���� ���ؼ� ��Ȯ�� ��ƼŬ�� �������� ������ ���� �����
* ������:
  * ���� ��ƼŬ�� size, color, opacity ���� lifetime ������� ���������� ������
  * ��� ������ ������ ����
    * ������ ������� ���� ���� �����ϵ��� ����
  * ���� ���� �߰�
  * �� ������ ����
* �����:
  * �� ������
  * ����1: <img src="./img/fire_example1.webp">
  * ����2: <img src="./img/fire_example2.webp">
  * ��ƼŬ�� ���� �������� ���� �������� �������� ������ ������.
    * ����: ExecuteIndirect ȣ�� �� �������� ���� �ִ� maxCommandsCount�� ������. (2��° ����)
    ``` c++
    // TODO: Optimization - Set proper NumMaxCommands.
    cmdList->ExecuteIndirect(
	    _commandSignature.Get(),
	    _resource->getMaxNumParticles(),
	    _resource->getIndirectCommandsResource(),
	    0,
	    _resource->getIndirectCommandsResource(),
	    _resource->getCommandBufferCounterOffset());
    ```
    * ExecuteIndirect ȣ�⿡�� commands�� ���� ���� counter buffer�� �Բ� �ѱ�Ƿ�, maxCommandsCount�� ���� indirectComamnds�� ������ ���� ������ ������ �� ���� ���� ���̶�� �����Ͽ�����, Ư�� GPU ����̹����� ������ maxCommandsCount�� �Ѱ��־�߸� ���� ������ �� �� ����(��ó: PIX ����� �м� ����)
    * �̿� ������ tasks�� ���� ����ȭ �ܰ迡�� �ذ��� ����
  * Ribbon, Trail rendering �غ�: Spline �н�
* �ݿ���:
  * Bitonic Sort�� ������ PIX Debugger�� Ȯ���ϴµ� �ڲ� �̻��� ����� ����
    * �ð������δ� ������ ���̴µ�, ����ŷ� ���� ������ ���� ������ ����� �� �Ǿ� �ִ� ��
    * �˰��� GPU�� �����ؼ� Nvidia Control Panel�� ���� ���� ������ ���־�� �ϴ� ���� �־���, �̰��� ����� �̷������ �ʾ� ������� �ʱ�ȭ�� ������ ���ܼ� �߻��� �������� ������.
  * Bitonic Sort�� 2�� n�� �÷θ� ������ �Ǿ, �̸� ������ n ũ�⿡�� ������ �� �ִ� ����� ã�ƺ�������..
    * [Arbitrary n size bitonic sort](https://hwlang.de/algorithmen/sortieren/bitonic/oddn.html) => �̰� �����غ� �� ������? �Ӹ��� ���ļ� ������ �׸��ξ���.
  * ��·�ų� Bitonic sort�� ���������� �̸� �ϴ� Ȱ���ϱ�� ��
    * 2^n���� ���� �ڸ����� ������ ���ļ��� ������ padding


##### 5����: Ribbon ������ (2023.11.06. ~ 2023.11.10.)
* ������:
  + ������ ���� ������ ����
  + <img src="./img/20231106_ribbon.png">
  + ���� Orphan nodes�� shader statements�� ��ȯ���� ����
* ȭ����:
  * ����
* ������:
  * ���� ������ �ؽ�ó ���� ����
* �����:
  * ���� ���������� distance based texture mapping ����
  * brent-kung parallel prefix sum �н� �� ����
* �ݿ���:
  * �ڵ� �����丵


##### 6����: �����丵 �� ���� ���� ȯ�� ���� (2023.11.13. ~ 2023.11.17.)
* ������:
  * �ڵ� �����丵
  * 4���� ����Ͽ� ���õǾ��� ���� �̽� �ذ�
  * ���� ���� ȯ�� ����
* ȭ����:
  * �̽� �߻�:
    * ����:
      * reserved�� ��ƼŬ ������ ���̸� �Ѿ�� �������� ��ƼŬ�� ������ ���� ���Ŀ� ����̽��� ���ŵ�.
      * ���� �� particle system�� �ִ� 1�� ���� ��ƼŬ�� ������ �� �ִٸ�, ���� ��ƼŬ�� ���� ��� ���� �뷫 1�� ���� ��ƼŬ�� ������ ���� ���Ŀ� ����̽��� ���ŵ�.
    * ���� ���:
      * DebugLayer���� [GPUbaseValidation Ȥ�� DRED auto-breadcrumb]�� �ϳ��� Ȱ��ȭ���� ���� ��(�� �� ��Ȱ��ȭ ��) �̻� ���� �߻�
      * �� �����̴���, ��������ȭ�� ��� �� ������ �߻����� ����
      * ������ ������ compute shader dispatch�� particle emission�� ���� ���� ��.
    * �������� ����: 
      * UAV�� �� ������ ����� ���� �ʴ� ������ ����
      * ����ִ� ��ƼŬ�� ���� �����ϴ� counter�� ���������� UAV �� �������� ���� �ִ� ��ƼŬ ���� �Ѿ�� �ǰ�, �̷� ���� out of bounds access�� �Ͼ�� ������ ����
* ������:
  * �̽� �ذ�:
    * ����: UAV�� ����ϴ� Dispatch ���̿� UAV Resource Barrier�� ���� ����
    * ����: GPU based validation�̳� auto-breadcrumb�� ��� �� �ڿ����� ���¸� �����ϱ� ���ؼ� ���ԵǴ� �ڵ��� ���� �̷��� ������ �巯���� ���� ���ϱ�?
  * IndirectDrawing Timing?
    * <img src="./img/pix_indirect_drawing_timing.png">
    * ExecuteIndirect ��� ������ ������������ �� gpu ���ް� Ȯ�ε� (���콺 Ŀ�� �κ��� ExecuteIndirect)
    * 100�� ���� ��ƼŬ�� �׸� ���ε�, drawing �ð��� ������������ ª�� ������ ���� gpu ���޷� ���̴� �κе��� ���� drawing�� Ȱ��Ǵ� �ð��̶� ����
    * Ȯ���� ���� Indirect Drawing�� �ÿ��ϴ� DirectX12 Sample�� timing ���� �����غ��Ұ� ���� ������ �� �� �־���.
  * 
* �����:
  * ���� ����
  * ��ƼŬ ���� ��� ���� ����
* �ݿ���:
  * Instancing�� �̿��ؼ� Ŀ�ǵ带 �����ϵ��� ����
    * 100�� �� sprite ������ 14ms -> 8ms


##### 7����: ���� ���� (2023.11.20. ~ 2023.11.24.)
* ������:
  * ��ƼŬ ���� ��� ����
  * ��ƼŬ ������ ��ŷ/����ŷ ����
  * �ȼ� ���̴����� ��ƼŬ ���۸� �������� �ʾƵ� �ǵ��� ���������� ����
* ȭ����:
  * ��ƼŬ ���� ��� ���濡 ���� ���� �� ��� ���� �䱸���� �ذ�
  * half type �������� ���� ���� 16 bit float packing ����


<hr/>

### ���� ���� ����
#### ��ƼŬ �ڿ�
* ��ƼŬ ������ ��� ���ۿ� �� �ε����� ��� ���� ������ 3�� ���
  + �ı��Ǿ� �ִ� ��ƼŬ���� �ε����� ��� ���� [deads]
    - ���� �� ������ ���� ���ְ�, �� ���Ҵ� 0������ �ִ� ��ƼŬ ������ �ʱ�ȭ�Ǿ� ����. 
  + ����ִ� ��ƼŬ���� �ε����� ��� ���� 2�� (ping-pong) [alives1, alives2]
* RWbyteaddressbuffer.InterlockedAdd�� �̿��� ���������� ��ƼŬ�� ���� ����ȭ

#### ��ƼŬ ����
* ��ƼŬ ���� �� �� ������� �ϳ��� ��ƼŬ�� ������.
* �̶� deads ������ �ֻ������� �׾� �ִ� ��ƼŬ�� �ε����� ��������, ������ ũ�⸦ 1 ���ҽ�Ŵ
* ��ƼŬ�� �ʱ�ȭ�ϰ�, alives ���ÿ� �ش� ��ƼŬ�� �ε����� ������.

#### ��ƼŬ �ı� �� �ùķ��̼�
* �� ������� alives ���ۿ��� �ϳ��� ��ƼŬ�� �ùķ��̼���.
* simulate �ܰ迡�� ��ƼŬ�� life�� 0 �̸��̸� deads ���ÿ� �ش� �ε����� ������.
* ���� ������ ����ִٸ� �ùķ��̼� ������ ��ģ �� ping-pong ������ alives2 ���ÿ� �ش� ��ƼŬ�� �ε����� ������.

#### ��ƼŬ ����
* Batcher's odd-even merge sort:
  + ������ ������ �����Ѵ�
  + ¦���� Ȧ�� �ε����� ������ �����Ѵ�.
  + (2l, 2l-1)�� ���� �����Ѵ�. ���� 8���� ������ ��� (2,3), (4,5), (6,7)
  + �̸� �ݺ��� ���� �Ϸ�
* Bitnoic sort:
  + descending, ascneding order�� �����ư��鼭 ������
  + �̵��� merge�ϴ� �۾����� ��������� �����Ͽ� ���� �Ϸ�
* Bitonic sort�� �� ���� ����?
  * data access ������ ��Ģ���̶� cache hit�� �� �̷���� �� ����.

#### ���̴� ���� ����
* ����:
``` c++
EmitCSBase.hlsl

// each thread emits a particle.
[numthreads(256, 1, 1)]
void EmitCS(
	int3 groupThreadId : SV_GroupThreadID,
	int3 dispatchThreadId : SV_DispatchThreadID)
{
	...
	%s

	newParticle.Color = float3(1.0f, 0.0f, 0.0f);

	// add particle into buffer
	// TODO: remove either numDeads or numAlives and derive it with max num of particles.
	uint numDeads;
	counters.InterlockedAdd(PARTICLECOUNTER_OFFSET_NUMDEADS, -1, numDeads);

	uint newParticleIndex = deadIndices[numDeads - 1];

	uint numAlives;
	counters.InterlockedAdd(PARTICLECOUNTER_OFFSET_NUMALIVES, 1, numAlives);
	
	aliveIndices[numAlives] = newParticleIndex;

	particles[newParticleIndex] = newParticle;
}
```

<img src="img/emitter_editor_ui_example.png">


``` c++
7765567160042113517.hlsl

// each thread emits a particle.
[numthreads(256, 1, 1)]
void EmitCS(
	int3 groupThreadId : SV_GroupThreadID,
	int3 dispatchThreadId : SV_DispatchThreadID)
{
    ...

float3 local6 = float3(0.000000, 0.000000, 0.000000);
newParticle.Position = local6;
float3 local4 = float3(random(0.293304 + DeltaTime + float(dispatchThreadId.x)), random(0.393304 + DeltaTime + float(dispatchThreadId.x)), random(0.493304 + DeltaTime + float(dispatchThreadId.x)));
float3 local3 = float3(0.000000, -1.000000, 0.000000);
newParticle.Acceleration = local3;
float local2 = float(3.000000);
newParticle.Lifetime = local2;
float local1 = float(0.050000);
newParticle.Opacity = local1;
newParticle.Size = local1;
float3 local0 = float3(-0.500000, -0.500000, -0.500000);
float3 local5 = local4 + local0;
newParticle.Velocity = local5;

	// add particle into buffer
	// TODO: remove either numDeads or numAlives and derive it with max num of particles.
	uint numDeads;
	counters.InterlockedAdd(PARTICLECOUNTER_OFFSET_NUMDEADS, -1, numDeads);

	uint newParticleIndex = deadIndices[numDeads - 1];

	uint numAlives;
	counters.InterlockedAdd(PARTICLECOUNTER_OFFSET_NUMALIVES, 1, numAlives);
	
	aliveIndices[numAlives] = newParticleIndex;

	particles[newParticleIndex] = newParticle;
}
```

* Base ���̴� ������ �ԷµǸ�, statements�� ���Ե� ������ "%s"�� ǥ��
* Graph �ڷᱸ���� ������� statements�� dependency�� ����
* �̵��� ���� ���� �� ���������� �ڵ忡 ����
  * ���� �켱 Ž���� ���� ������ �Ųٷ� ��ȸ
* Ŭ���̾�Ʈ�� HlslGenerator ��ü�� �Լ��� �̿��� ���ϴ� ���� ����
  * ��ü�����δ� statement node�� ���� �� �׵� ���� ���踦 ����
  * �� statement node�� ������ ��ȣ�� ����
  * ����:
``` c++
UINT positionIndex = _hlslGenerator->newFloat3(0.0f, 0.0f, 0.0f);

UINT randFloat3 = _hlslGenerator->randFloat3();
UINT minusHalfFloat3 = _hlslGenerator->newFloat3(-0.5f, -0.5f, -0.5f);
UINT velocityIndex = _hlslGenerator->addFloat3(randFloat3, minusHalfFloat3);

UINT accelerationIndex = _hlslGenerator->newFloat3(0.0f, -1.0f, 0.0f);

UINT lifetimeIndex = _hlslGenerator->newFloat(4.0f);

UINT sizeIndex = _hlslGenerator->newFloat(0.05f);

UINT opacityIndex = _hlslGenerator->newFloat(1.0f);

_hlslGenerator->setInitialPosition(positionIndex);
_hlslGenerator->setInitialVelocity(velocityIndex);
_hlslGenerator->setInitialAcceleration(accelerationIndex);
_hlslGenerator->setInitialLifetime(lifetimeIndex);
_hlslGenerator->setInitialSize(sizeIndex);
_hlslGenerator->setInitialOpacity(opacityIndex);

compileShaders();
```
* �̶� ���̴��� ���� ��� ���� ������ ������ ����.
  * ���� EmitCS�� ��� InitalPosition, InitialVelocity ������ ���� ������� ����.
* ������ Shader ���ϵ��� �̸��� ��ü ���� ������ �ؽ��� ������ ����
* 

#### �� ������
* 3���� ������ �ؽ�ó�� ���� (R32G32B32FLOAT)
  * Perlin Noise�� �̿��� �����̾�����, �׳� �ܼ� noise�� ����ص� ������ ���� ����� ���̹Ƿ� ���ķ� �̷����
  * <img src="./img/noise_texture.png">
* �̴� ���Ͱ� �Ǹ� ��ƼŬ�� ��ġ�� ������� �ؽ�ó�� linear sample�� ���� ������ velocity�� ����
* <img src="./img/curl_noise.webp">


#### �� ������

* ����1: <img src="./img/fire_example1.webp">
* ����2: <img src="./img/fire_example2.webp">
* ��� ������ ����:
  * <img src="./img/fire_example1_ps0_emitter.png">
  * <img src="./img/fire_example1_ps0_simulator.png">
  * <img src="./img/fire_example1_ps0_renderer.png">
* ���� ���
  * Particle systems
    * ���� �ö󰡴� �Ҳ��� ǥ���ϴ� particle system
    * ���⸦ ǥ���ϴ� particle system
    * �Ҿ��� ǥ���ϴ� particle system
    * �߾� �Ҳ��� ǥ���ϴ� particle system
  * �Ҳɰ� ����� subuv �ؽ�ó�� �̿��ؼ� lifetime�� ������� �ִϸ����õ�.
    * <img src="./FullyGpuParticleSystem/Textures/fire_subuv.png">
    * �ִϸ��̼� ������ ���� �ʾ���!

#### ���� ������
* 1���� ���� ��ǥ:
  * https://youtu.be/Zn9-U5vQl3g?si=OwzghEU3Dq-Bh-fX&t=155
  * Spawntime ������� particle ���� ��
  * �̵��� ������ spline(Catmull-rom) ������
  * ������ ���۷����� ã�� �������Ƿ�, �� ����� �����Ѵ�!
* ����
  * Spawntime���� �Ϸ��� ������, �߰����� ���ڰ� �ʿ���. �ֳ��ϸ�:
    * �� �����ӿ��� ���� spawntime�� ���� ���� ��ƼŬ���� ������
    * Bitonic sort�� stable�� sort�� �ƴ� ���̹Ƿ�, spawntime�� ���� ��ƼŬ���� ����� ������ �����ȴٴ� ������ ����
    * ���� spawn �� �ش� dispatch thread id�� ����ϰ� �� ���� ���Ŀ� �����ϵ��� ����
* Ribbon ����
  * Tessellation�� �̿�
    * �Է�: 4���� Control points C0, C1, C2, C3
    * C1�� C2 �� �Ʒ��� ���� ������ quad ���� �� tessellation
    * <img src="./img/ribbon_hull_shader.png">
    * Catmull-rom spline�� �̿��� C1�� C2 ���� ���е� ������ ��ġ�� ����
    ``` C++
    ...
	float t = uv.x;

	float3 upPosU = catmullRom(
		ribbonPatch[0].ControlPoint0,
		ribbonPatch[0].ControlPoint1,
		ribbonPatch[0].ControlPoint2,
		ribbonPatch[0].ControlPoint3,
		t);

	float3 downPosU = catmullRom(
		ribbonPatch[1].ControlPoint0,
		ribbonPatch[1].ControlPoint1,
		ribbonPatch[1].ControlPoint2,
		ribbonPatch[1].ControlPoint3,
		t);

	float3 pos = lerp(upPosU, downPosU, uv.y);
    ...
    ```
  * Control Points �Է� ���
    * Index View Buffer�� 0���� maxParticleCount���� ä���� ����
    * Indirect dispatch�� �̿��� control points�� �Է� �����⿡ ����
        ``` C++
        [numthreads(256, 1, 1)]
        void RibbonComputeIndirectCommandsCS(int3 dispatchThreadId : SV_DispatchThreadID)
        {
	        uint id = dispatchThreadId.x;
	        uint numAlives = counters.Load(PARTICLECOUNTER_OFFSET_NUMALIVES);

	        if (id >= numAlives - 3 || numAlives <= 3)
	        {
		        return;
	        }

	        IndirectCommand command;
	        command.IndexCountPerInstance = 4;
	        command.InstanceCount = 1;
	        command.StartIndexLocation = id;
	        command.BaseVertexLocation = 0;
	        command.StartInstanceLocation = 0;

	        outputCommands.Append(command);
        }
        ```
    * <img src="./img/ribbon_input.png">
    * ������: 0\~1�� n-1\~n�� ribbon�� ������� ����.
    * ���� index buffer�� �� �κп� 0�� �߰��� �����Ͽ� ù ��° ribbon�� ��� control point�� 0, 0, 1, 2�� �Էµǰ� �Ͽ� 0~1�� ��ƼŬ�� �հ� ��.
    * ���������� ������ ribbon�� ��� numAlives�� Ȯ���ؼ� �ش� index�� �����ϰԲ� �� (n-2, n-1, n, n+1) => (n-2, n-1, n, n)
    * �̴� vid�� �Է��� �Ǹ�, �̸� �ε����� �̿��� particle buffer�� ����
  * Color, Opacity interpolation
    * Pixel shader���� �� segment�� uv���� �����ϸ�, �̸� �̿��� spline ������ �Ͱ� ���� ������� ����.
  * Texture Coordinate
    * ������ ���� texture: 
      * <img src="./FullyGpuParticleSystem/Textures/uv_mapper.png">
    * 3���� ���·� Texture mapping�� �غ�����: Segment based, Stretched, Distance based
    * Segment based
      * �� ��ƼŬ���� �մ� �ϳ��� segment�� 0~1�� Texture Coordinate�� ����
      * <img src="./img/ribbon_segment_based.png">
      * <img src="./img/ribbon_texture_segment.webp">
    * Stretched
      * ���� ��ƼŬ���� �� ��ƼŬ���� 0~1�� Texture Coordinate�� ����
      * <img src="./img/ribbon_stretched.png">
      * <img src="./img/ribbon_texture_stretched.webp">
    * Distance based
      * ���� ��ǥ �������� ��ƼŬ ������ �Ÿ��� �ٻ��� �Ÿ��� �������� texture coordinate�� �ο��� tiling��
      * <img src="./img/ribbon_distance_based.png">
      * <img src="./img/ribbon_texture_distancebased.webp">
      * Catmull-rom spline�� �̷�� ȣ�� ���̸� ���غ��� ������ �̴� �����
      * ��������� spline�� ����� �������� ũ�� �ʰ� ���� ��ȯ�� �ް��ϰ� �̷�����Ƿ�, �ܼ��� ���� �Ÿ��� �̿��ؼ� �ٻ��ص� ������ ������� ���� ���̶�� �����Ͽ���
      * i��° ��ƼŬ�� i-1��° ��ƼŬ ������ �Ÿ��� ���� ���ϰ� �̸� ������ ��ƼŬ���� ������ ����������, �̰͸����δ� ������
        * �ֳ��ϸ� tiling�� �ڿ������� �̷������ �ϱ� ���ؼ��� ���� segment�� �ؽ�ó ��ǥ���� �˾ƾ� �ϱ� ����!
        * ��, "������"�� �ʿ���
        * <img src="./img/ribbon_distance_based_why_prefix_sum.png">
      * �������� Brent-kung adder�� �̿��� ���������� �����Ͽ���
        * <img src="./img/brent_kung_adder.png">
      * �̸� ���� ù ��ƼŬ�κ��� �� ��ƼŬ������ �Ÿ��� ����� �� ������, �� ���� �״��(Ȥ�� scaling�Ͽ�) texture coordinate�� Ȱ����

#### ���� ���� ���
* PIX Debugger�� Ȱ���Ͽ��� (�� �н����� �̺�Ʈ ��ŷ) [WinPixEventRuntime](https://devblogs.microsoft.com/pix/winpixeventruntime/)
* �پ��� ��Ȳ�� ����Ͽ� �ð��� �����Ͽ���
  * ��ƼŬ�� ��
  * ������ ���(��������Ʈ, ����)
  * ���� ���(������, ������)
  * ��ƼŬ ũ���� ��� opaque���� �������� ū ������ ��ġ�� �ʾ� ������� ���� (additive blending�� ��� ����ؼ� �þ)
* ���� ���� ȯ�� ����
  * �ػ�: FHD (���� �ణ ����)
  * ��ƼŬ�� ȭ�� ��ü�� �����Ǿ� ������
  * Lifetime: 3s
  * ����
    * Sprite
      * <img src="./img/performance_test_sprite_example.png">
    * Ribbon
      * <img src="./img/performance_test_ribbon_example.png">
* ���� ���� 
  * ���� ����

    | \# of Particles | Renderer Type | Blending Type | Ribbon UV Mode    | Emission | Simulation | Post-simulation | Pre-sort | Sort     | Pre-prefix Sum | Prefix Sum | Computing Indirect Commands | Indirect Drawing | Total     |
    | --------------- | ------------- | ------------- | ----------------- | -------- | ---------- | --------------- | -------- | -------- | -------------- | ---------- | --------------------------- | ---------------- | --------- |
    | 1,000,000       | Sprite        | Opaque        | \-                | 0.074592 | <b>11.902528</b> | 0.001088        | 0.000000 | 0.000000 | 0.000000       | 0.000000   | 0.516608                    | <b>13.619712</b>        | 26.114528 |
    | 10,000          | Ribbon        | Opaque        | Segment based     | 0.005984 | 0.057088   | 0.001088        | 0.042048 | 0.450976 | 0.000000       | 0.000000   | 0.032448                    | <b>4.077952</b>         | 4.667584  |
    | 10,000          | Ribbon        | Opaque        | Distance based UV | 0.005216 | 0.059200   | 0.001088        | 0.042208 | 0.463328 | 0.038752       | 0.873888   | 0.032352                    | <b>3.980608</b>         | 5.496640  |

* ��ƼŬ �ý��� ����
  * <img src="./img/particle_system_before.png">
  * ~~1���� ��ǥ: numDeads�� numAlives�� maxParticleCount�� ���ؼ� ������ �� �ִ�. (atomic operation �ʿ� X)~~
  * ~~����: ������ �ε������� �ٷ�� �̸� ������� ��ƼŬ ���۸� �����ϰ� �ִµ�, �ε����� �ƴ϶� ���� ��ƼŬ ���۸� �ٷ�� ���� �ٷ�� ���·� �ٲٸ� ������ ���� �����ɱ�?~~
  * Spawnrate, lifetime�� �˸� ���� ��ƼŬ���� ���� ������ �� �����Ƿ� ��� ��ƼŬ�鿡 ���ؼ� ������ Ȯ���� �ʿ䰡 ����.
    * ���� �ּ����� ��ƼŬ�鸸 atomic�ϰ� ping-pong buffer�� �ű�� �������� parallel�ϰ� �̵�
    * <img src="./img/particle_system_after.png">
    * ������ simulation ������ ������ ������: ��ƼŬ�� �ű� ��, simulation�� �� �� �� �� particle ���ۿ� ���� ������ �ϰ� �Ǿ��� ����.
    * �ٸ� ��ƼŬ���� �����ϰ� �̵��ǹǷ� sort �ҿ䰡 ������.
    * ���� ��:
   
    | \# of Particles | Renderer Type  | Ribbon UV Mode    | Emission | Simulation | Post-simulation | Pre-sort     | Sort       | Pre-prefix Sum | Prefix Sum | Computing Indirect Commands | Indirect Drawing | Total     |
    | --------------- | -------------  | ----------------- | -------- | ---------- | --------------- | ------------ | ---------- | -------------- | ---------- | --------------------------- | ---------------- | --------- |
    | 1,000,000       | Sprite         | \-                | 0.032384 | <b>7.684832</b>   | 0.001088        | 0.000000     | 0.000000   | 0.000000       | 0.000000   | 0.001728                    | <b>8.164768</b>         | 15.884800 |
    | 10,000          | Ribbon         | Distance based UV | 0.005568 | 0.126208   | 0.001088        | <b>4.707200</b>     | <b>8.149344</b>   | 0.374080       | 0.521722   | 0.001888                    | <b>3.070464</b>         | 16.957562 |

    * ���� ��:
 
    | \# of Particles | Renderer Type | Ribbon UV Mode    | Emission | Destroy    | MoveAlives      | Post-Destroy | Simulation | Pre-prefix Sum | Prefix Sum | Computing Indirect Commands | Indirect Drawing | Total     |
    | --------------- | ------------- | ----------------- | -------- | ---------- | --------------- | ------------ | ---------- | -------------- | ---------- | --------------------------- | ---------------- | --------- |
    | 1,000,000       | Sprite        | \-                | 0.043008 | 0.035520   | <b>7.388256</b>        | 0.000445     | 1.781856   | 0.000000       | 0.000000   | 0.000992                    | <b>8.482816</b>         | 17.732893 |
    | 10,000          | Ribbon        | Distance based UV | 0.005728 | 0.001600   | 0.087872        | 0.000544     | 0.046208   | 0.045984       | 0.567296   | 0.001120                    | <b>4.250624</b>         | 5.006976  |

  * memory bandwidth�� �ּ�ȭ�ϱ� ���� ��ƼŬ ����ü�� ũ�⸦ ����.
    * ���� ��: 112 byte
    ``` C++
    struct Particle
    {
	    float3 Position;
	    float InitialSize;

	    float3 Velocity;
	    float InitialLifetime;

	    float3 Acceleration;
	    float InitialOpacity;

	    float3 InitialColor;
	    float RemainLifetime;

	    float3 EndColor;
	    float EndSize;

	    float EndOpacity;
	    float SpawnTime;
	    uint SpawnOrderInFrame;
	    float DistanceFromPrevious;

	    float DistanceFromStart;
	    float3 Pad;
    };
    ```
    * ���� ��: 40 byte

    ``` C++
    struct Particle
    {
	    uint PositionXY;
	    uint PositionZVelocityX;
	    uint VelocityYZ;
	    uint InitialLifetimeAndRemainLifetime;
	    //----//
	    uint AccelerationXY;
	    uint AccelerationZAndSpawnTime;
	    uint InitialSizeAndEndSize;
	    uint DistanceFromPreviousAndDistanceFromStart;
	    //----//
	    uint InitialColor;
	    uint EndColor;
    };
    ```
    * Color�� ��� 8bit 4���� packing
    ```
    uint packUnorm4ToUint(float4 unpackedData)
    {
	    uint result = 0;

	    result += uint(unpackedData.x * 255.0f);
	    result <<= 8;
	    result += uint(unpackedData.y * 255.0f);
	    result <<= 8;
	    result += uint(unpackedData.z * 255.0f);
	    result <<= 8;
	    result += uint(unpackedData.w * 255.0f);

	    return result;
    }

    float4 unpackUintToUnorm4(uint packedData)
    {
	    float4 result;

	    result.x = float( packedData		>> 24)	 * 0.00392156f;
	    result.y = float((packedData & 0x00FF0000)	>> 16)	 * 0.00392156f;
	    result.z = float((packedData & 0x0000FF00)	>> 8 )	 * 0.00392156f;
	    result.w = float( packedData & 0x000000FF        )	 * 0.00392156f;

	    return result;
    }

    uint packFloat2ToUint(float a, float b)
    {
	    uint a16 = f32tof16(a);
	    uint b16 = f32tof16(b);
	    return (a16 << 16) | b16;
    }

    void unpackUintToFloat2(uint input, out float a, out float b)
    {
	    a = f16tof32(input >> 16);
	    b = f16tof32(input & 0x0000FFFF);
    }
    ```
    * ��:

    | Particle Size            | MoveAlives | Simulation |
    | ------------------------ | ---------- | ---------- |
    | 112byte (16byte aligned) | 7.388256   | 1.781856   |
    | 80byte (16byte aligned)  | 4.612512   | 1.583840   |
    | 72byte                   | 3.995648   | 1.902184   |
    | 64byte (16byte aligned)  | 2.971968   | 0.891296   |
    | 48byte (16byte aligned)  | 2.159088   | 0.994336   |
    | 44byte                   | 2.040000   | 1.039904   |
    | 40byte                   | 1.737888   | 0.893024   |

    * ��ƼŬ�� �����ϴ� ��뿡 ���� �м�:
      * ������ ������ 16 byte stride�� ��Ű�� ���� ���ɿ� �����ϴٰ� �ϴµ�, �� ��쿡�� �ܼ��� byte size�� ���� ���� ��������.
        * [Understanding Structured Buffer Performance](https://developer.nvidia.com/content/understanding-structured-buffer-performance)
        * �ٸ� simulation�ʿ����� �ش� ��Ģ�� �� �������� ������ ����.
      * ���� ����� coalescing, access pattern �� �پ��� ��Ұ� �ۿ��ߴٰ� ���̴µ�, �ϵ������ ���� �������� ��Ȯ�ϰ� ¤�� ���� ���� �� ����.
      * ����:
        * 64, 48, 44, 40�� simluation �ӵ��� ����: memory access pattern�� ������ �� ������?


### Bounding Mode
* Sprite�� texture���� opacity�� �ְ� �̸� �̿��� �ȼ��� �Ⱒ�� �� ����
  * ������ ȭ�鿡�� �׷����� ��κ� �ȼ��� opacity�� 0�� ���, overdraw�� �߻���Ŵ.
  * <img src="./img/scratch.png">
  * ���� quad�� �ƴ� bounding n-gon�� ������ �׸��� ���� �� ������ �� ����:
  * <img src="./img/bounding_scratch_wireframe.png">
  * ����:
    * �ؽ�ó�κ��� ���� �� �� �ִ� �ȼ����� ����
      * �� �ึ�� ���� ������ opacity�� threshold �̻��� �ȼ��� ���� (N by N �ؽ�ó�� ��� �ִ� 2N ���� �ȼ� ����)
    * ���õ� �ȼ����� ������� convex hull ����
      * Braham scan �˰����� ������� �Ͽ���
    * ������ convex hull�� ������� bounding n-gon�� ����
      * ���� n�� 8�� hard-coded
    * ���� ��
      * ��Ȳ1: ��ü ��ƼŬ�� 3500��, �ϳ��� ��ƼŬ�� quad �������� �뷫 ȭ���� 15% �̻� ä���, ���� �̻��� �Ⱒ��. ���� �׸� ����:
        * <img src="./img/bounding_example.png">
        * �ؽ�ó:
          * <img src="./img/circle.png">
        * ����ǥ:

        |              | \# of Particles | Renderer Type | Blending Type | Indirect Drawing |
        | ------------ | --------------- | ------------- | ------------- | ---------------- |
        | Bounding ����X | 3,500           | Sprite        | Additive      | 20.674528        |
        | Bounding ����O | 3,500           | Sprite        | Additive      | 16.544576        |
      * ��Ȳ2: ��ü ��ƼŬ�� 100�� ��, ��ü������ �����־� �ϳ��� ��ƼŬ�� ȭ���� �� �ȼ� �������� ����.
        * <img src="./img/bounding_example_2.png">
        * ����ǥ:

        |                | \# of Particles | Renderer Type | Blending Type | Indirect Drawing |
        | -------------- | --------------- | ------------- | ------------- | ---------------- |
        | Bounding ����X | 1,000,000       | Sprite        | Additive      | 7.412736         |
        | Bounding ����O | 1,000,000       | Sprite        | Additive      | 9.781248         |
  * ���
    * ������ �� ���������� �Ⱒ�Ǵ� �ȼ��� ���� �� ������ �� ����.
    * Quad�� �ƴ� n-gon�� �����ϴµ� ��� ����� ������ ����ؾ� �� ��.
    * ���� ���� ����� �̷��� bounding�� �ʿ������ �ؽ�ó �� ��ƼŬ ȿ���� ������ ��Ƽ��Ʈ�� ������ �޴� ��!




<hr/>

### ������ �� �ڵ�
* Particle System
  * [William T. Reeves, particle systems - a technique for modeling a class of fuzzy objects](https://www.lri.fr/~mbl/ENS/IG2/devoir2/files/docs/fuzzyParticles.pdf)
  * [Lutz Lata, Building a Million Particle System](https://citeseerx.ist.psu.edu/document?repid=rep1&type=pdf&doi=b5aa42d88a178b264f4ceb3ddb67d3d00ecbc631)
  * [Karl Sims, Particle animation and rendering using data parallel computation](https://www.karlsims.com/papers/ParticlesSiggraph90.pdf)
  * [GPU based paritlce system - Wicked engine](https://wickedengine.net/2017/11/07/gpu-based-particle-simulation/)
  * DirectX12 Mini Engine

* Parallel sort
  * [Bathcer's Algorithm](https://math.mit.edu/~shor/18.310/batcher.pdf)
  * [Bitonic sorter](https://en.wikipedia.org/wiki/Bitonic_sorter)

* Ribbon Trail
  * [Ribbon and Trail](https://doc.stride3d.net/4.0/en/manual/particles/ribbons-and-trails.html)
  * [Smooth Particle Ribbons Through Hardware Accelerated Tessellation](https://www.diva-portal.org/smash/get/diva2:1692949/FULLTEXT01.pdf) - ���⿡ ���� ������� Ȱ�������� �ʾ���.
  * [Bezier Curve](https://www.particleincell.com/2012/bezier-splines/)
  * [Catmull-Rom Curve](https://en.wikipedia.org/wiki/Centripetal_Catmull%E2%80%93Rom_spline)
  * [Catmull-Rom Spline](https://www.mvps.org/directx/articles/catmull/)
  * [Parallel Prefix Sum - Nvidia](https://developer.nvidia.com/gpugems/gpugems3/part-vi-gpu-computing/chapter-39-parallel-prefix-sum-scan-cuda)
  * [Parallel Prefix Sum - Open Source in Github](https://github.com/b0nes164/GPUPrefixSums/tree/main)

* Perlin Noise
  * [Perlin Noise](https://en.wikipedia.org/wiki/Perlin_noise)

* Particle Trimming (Bounding)
  * [Alok Aggarwal, Minimum area circumscrbinig Polygons]
