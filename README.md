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
* Ribbon trail renderer
* Mesh(i.e. cube) render
* Curl Noise simulation
* Particle�� Orientation �Ӽ� �߰�
* Opaque particles�� ���� draw�ؾ� ��
* ������ ��� ���
* Motion blur
* Collision
* Lighting
* Shader generation���� node dependency ���Ἲ Ȯ��
  * i.e.) float3�� float4�� ���������� �ʴ���?
  * �� node���� type ������ �����Ͽ� ������ input���� �����ؾ� ��

##### ���� ���� Task
* Particle System �н�
* UI ����

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
* Simulation Node:
  * Vortex Force
  * Point Attraction Force
  * Drag Force
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

