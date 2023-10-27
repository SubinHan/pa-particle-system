## Particle System

### 목차
[프로젝트 개요](#프로젝트-개요) <br>
[목표 및 계획](#목표-및-계획) <br>
[Tasks](#Tasks) <br>
[일지](#일지) <br>
[세부 구현 설명](#세부-구현-설명) <br>
[참고문헌](#참고문헌)

<hr/>

### 프로젝트 개요

단순한 수준의 파티클 시스템을 D3D12로 구현합니다. <br>
특히, **Simulation이 아닌 Particle Control에 집중**합니다. <br>


#### 구현 목표 Reference
* ***Unreal Engine 5 Niagara*** <br>
* [YouTube: ncParticleEditor](https://www.youtube.com/watch?v=RLNI5NMCJ1E) <br>
* [YouTube: OpenGL/Imgui Engine Build Stage 3.5: Particle System](https://www.youtube.com/watch?v=rZ3ztv7u0Yk) <br>
* [YouTube: 2D Game Particle tool using IMGUI in DirectX11](https://www.youtube.com/watch?v=TXf4D8Ess6s) <br>
* [YouTube: Compute Shader Particle System pt. 3, GUI](https://www.youtube.com/watch?v=UqB4paFZcBg) <br>
* [YouTube: Custom Particle Editor](https://www.youtube.com/watch?v=iQhOHd_JSgk) <br>

<hr/>

### 목표 및 계획

##### 1주차: 계획 선정 및 개발 환경 구성
* 주제 선정 및 조사
* D3D12 개발 환경 구성


##### 2~4주차: 핵심 기능 구현 및 Particle System 학습

* 학습
* D3D12 기초적 렌더러 구현
* emnitter, particle 등 핵심 기능 설계 및 구현
* Computing Shader 작성
  + EmitterCS 작성
  + SimulateCS 작성
* Particle Rendering 구현
  + VS, GS, PS 작성
* 단순한 수준의 particle simulation 구현 (i.e. sprinkler)
* z-buffer에 의한 particle occlusion
* *motion blur 구현 (진행 상황에 따라 수행)*
* *alpha-blending (진행 상황에 따라 수행)*
  + *SortingCS 작성*
  + *alpha-blending 구현*
* *geometry와의 collision (진행 상황에 따라 수행)*

##### 5~7주차: UI 도입 및 사용성, 확장성, 유지보수성 증대

* imgui 도입
* hard coding 되어 있을 다양한 값들 parameter화
* 유연한 particle control을 위한, 핵심 기능의 구조 개선
* 다양한 particle contorl을 위한, 기능 구현 (i.e. gravity, drag, noise, wind, ...)


##### 8주차: 마무리
* refactoring
* visuality 개선

#### 위험 요소
* Native한 D3D12로 진행 (Mini Engine 등 사용 X)
* Particle System 학습 미비
* imgui 사용 경험 없음

<hr/>

### Tasks

##### Backlogs
* Ribbon trail renderer
* Mesh(i.e. cube) render
* Curl Noise simulation
* Particle에 Orientation 속성 추가
* Opaque particles를 먼저 draw해야 함
* 프레임 통계 모듈
* Motion blur
* Collision
* Lighting
* Shader generation에서 node dependency 무결성 확인
  * i.e.) float3에 float4를 대입하지는 않는지?
  * 각 node들이 type 정보를 유지하여 적절한 input임을 검증해야 함

##### 진행 중인 Task
* Particle System 학습
* UI 적용

##### 완료된 Tasks
* D3D12 개발 환경 구성 (+PIX 디버거)
* D3D12 기초적 렌더러 구현
* 간단한 수준의 Particle 생성/파괴, 시뮬레이션을 위한 Computing Shader 작성
* 간단한 수준의 Particle System을 위한 렌더링 파이프라인 구성
* 간단한 수준의 emitter, particle 등 핵심 기능 설계 및 구현
* Texture mapping 수행
* alpha blending 수행
* Sorting 구현 확인
  + Batcher's odd-even merge sort 학습
  + Bitonic sort 학습
  + 간단한 구현 수행 및 결과 확인
* HLSL Generator 구현
  * 추상화된 작업들을 HLSL 코드로 변환시켜주는 클래스
  * 가령 float4 변수를 초기화하고, 두 개의 float4 변수를 더하는 것 등
  * 이를 기반으로 유연한 Emit, Simulate process 구현의 기반 마련
* DrawIndexedIndirect(), ExecuteIndirect() 학습 및 적용
* UI:
  * Spawn rate 조절 가능
  * Emitter Shader 작성 가능
  * Simulator Shader 작성 가능
  * Renderer Shader 작성 가능
  * 새로운 Particle System 추가 가능
  * Particle System의 상태를 저장 및 로드
  * Opaque/Translucent 변환 가능
* Simulation Node:
  * Vortex Force
  * Point Attraction Force
  * Drag Force
* 

<hr/>

### 일지
 
##### 1주차: 계획 선정 및 개발 환경 구성 (2023.10.10. ~ 2023.10.13.)
* 수요일:
* 목요일:
  + D3D12 환경 구성
  + 렌더링 파이프라인 구성
* 금요일: 
  + PIX 디버거 숙지 및 환경 구성
  + 정육면체 렌더링 완료
  + 의사 난수 (https://stackoverflow.com/questions/4200224/random-noise-functions-for-glsl)
  + Emitter, Particle 자료구조 prototype
  + ParticleEmitterCS, ParticleSimulateCS prototype

<br> 

##### 2주차: 핵심 기능 구현 및 Particle System 학습 (2023.10.16. ~ 2023.10.20.)
* 월요일:
  + ParticleEmitterCS, ParticleSimulateCS 작성
  + Particle 렌더링을 위한 쉐이더 일부 작성
  + ParticleSimulateCS 버그: RWByteAddressBuffer의 InterlockedAdd, Store, Load 사용 코드에 문제가 있어 동기화가 제대로 수행되지 않음 - 해결: 쉐이더 코드가 아닌 CPU 코드에 문제가 있었음. 메모리 주소를 잘못 넘겨주어 비정상적인 값이 덮어 씌워지고 있었음.
* 화요일:
  + 여전히 RWByteAddressBuffer와 관련한 문제가 있었음: 단순히 UnorderedAccessBuffer로서 RootSiganture에서 다루어졌지만, RawBuffer는 반드시 UnorderedAccessView를 통해 DXGI_FORMAT_R32_TYPELESS 타입과 D3D12_BUFFER_UAV_FLAG_RAW 플래그를 지정해 다루어야 함! 지식이 부족해 이를 적용하지 않았고 문제가 발생하였던 것이었음.
  + 기본적인 파티클 렌더링 수행 완료 (100만 개)
    <img src="./img/20231017_particles.png">
  + 정해진 최대 파티클 수량을 넘을 시 생성하지 않도록 제한
* 수요일:
  + Index Buffer 타입이 16bit uint라 2^16 이상의 파티클을 다루면 문제가 발생하여 해결.
  + Particle의 평면이 항상 Camera를 바라보도록 함
  + ParticleSystem 클래스 설계 및 구현
  + 여러 개의 ParticleSystem을 관리할 수 있음 (아직은 각각의 particle system이 서로 다른 world transform만 가질 수 있음)
  + 간단한 텍스처 매핑 (성능: 대략 10만 개)
    <img src="./img/20231018_particles.png">
* 목요일:
  + 알파 블렌딩 수행
    <img src="./img/20231019_particles_alphablending.png">
  + Spawn rate 조절 가능
  + Batcher's odd-even merge sort 학습
  + Bitonic sort 학습
  + Bitonic sort prototype (단순히 인덱스를 기준으로 정렬해보았음)
* 금요일:
  * 렌더링과 관련 없는 utils의 자동화 유닛 테스트 환경 구축
  * HlslTranslator (ParticleEmitter에만 시험적으로 적용)
    * Graph 자료구조 기반으로 hlsl을 생성하는 클래스
    * base shader 파일을 기반으로 특정 지점에 동적으로 코드 삽입 후 컴파일
  * Object hashing
    * 각 particle system 마다의 shader 관리를 위한 object 수준에서의 hashing 구현
    * 객체 생성 시점을 이용해 hashing 
  * DrawIndexedIndirect(), ExecuteIndirect() 학습

<br>

##### 3주차: 동적 쉐이더 컴파일 구현 및 UI 도입 (2023.10.23. ~ 2023.10.27.)
* 월요일:
  + Indirect Drawing 도입 (기존에는 무조건 최대 파티클 수만큼 draw call 생성)
  + 동적 쉐이더 컴파일 적용
    + Emit, Simulate, Render 단계에서의 연산을 동적으로 제어 가능
  + Imgui 학습 및 예제 적용
* 화요일:
  * imgui 도입 (+node graph 관련 library인 imnodes)
  * Emitter에 시험적 node editor 적용
    * node editor 기반으로 쉐이더 수정 가능
* 수요일:
  * UI 적용
    * Particle Emission, Simulation, Rendering 변경 가능
    * 노드 에디터의 정보들은 저장됨
      <img src="./img/20231025_ui.png">
* 목요일:
  * UI 적용
    * Particle System들의 상태를 저장할 수 있음
      * 다시 프로그램 시작 시 자동적으로 load
* 금요일:
  * Simulation Node 추가
    * Drag Force
    * Vortex Force
    * Point Attraction Force
  * Simulation 방식 변경
    * Velocity, Acceleration, Position을 누적해 변경하고 마지막에 Solving 됨
  * UI 적용
    * Node를 삭제할 수 있음
    * 각 Emitter, Simulator, Renderer Editor에서 사용할 수 있는 node들만 생성할 수 있음
  * <img src="./img/20231027_particles.webp">

<hr/>

### 세부 구현 설명
#### 파티클 자원
* 파티클 정보를 담는 버퍼와 그 인덱스를 담기 위한 스택을 3개 사용
  + 파괴되어 있는 파티클들의 인덱스를 담는 스택 [deads]
    - 시작 시 스택이 가득 차있고, 각 원소는 0번부터 최대 파티클 수까지 초기화되어 있음. 
  + 살아있는 파티클들의 인덱스를 담는 스택 2개 (ping-pong) [alives1, alives2]
* RWbyteaddressbuffer.InterlockedAdd를 이용해 전역적으로 파티클의 수를 동기화

#### 파티클 생성
* 파티클 생성 시 각 스레드는 하나의 파티클을 생성함.
* 이때 deads 스택의 최상위에서 죽어 있는 파티클의 인덱스를 가져오며, 스택의 크기를 1 감소시킴
* 파티클을 초기화하고, alives 스택에 해당 파티클의 인덱스를 삽입함.

#### 파티클 파괴 및 시뮬레이션
* 각 스레드는 alives 버퍼에서 하나의 파티클을 시뮬레이션함.
* simulate 단계에서 파티클의 life가 0 미만이면 deads 스택에 해당 인덱스를 삽입함.
* 만약 여전히 살아있다면 시뮬레이션 연산을 마친 뒤 ping-pong 버퍼인 alives2 스택에 해당 파티클의 인덱스를 삽입함.

#### 파티클 정렬
* Batcher's odd-even merge sort:
  + 반절로 나누어 정렬한다
  + 짝수와 홀수 인덱스로 나누어 정렬한다.
  + (2l, 2l-1)을 비교해 정렬한다. 가령 8개의 원소일 경우 (2,3), (4,5), (6,7)
  + 이를 반복해 정렬 완료
* Bitnoic sort:
  + descending, ascneding order를 번갈아가면서 정렬함
  + 이들을 merge하는 작업들을 재귀적으로 수행하여 정렬 완료
* Bitonic sort가 더 빠른 이유?
  * data access 패턴이 규칙적이라 cache hit가 잘 이루어질 수 있음.

#### 쉐이더 동적 생성
* 예시:
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

* Base 쉐이더 파일이 입력되며, statements가 삽입될 지점은 "%s"로 표시
* Graph 자료구조를 기반으로 statements의 dependency를 관리
* 이들을 위상 정렬 후 순차적으로 코드에 삽입
  * 깊이 우선 탐색의 후위 순서를 거꾸로 순회
* 클라이언트는 HlslGenerator 객체의 함수를 이용해 원하는 로직 생성
  * 구체적으로는 statement node를 생성 후 그들 간의 관계를 정의
  * 각 statement node는 고유한 번호가 존재
  * 예시:
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
* 이때 쉐이더의 최종 출력 값은 사전에 정해져 있음.
  * 가령 EmitCS의 경우 InitalPosition, InitialVelocity 따위를 최종 출력으로 받음.
* 생성된 Shader 파일들의 이름은 객체 생성 시점을 해싱한 값으로 구분
* 
<hr/>



### 참고문헌 및 코드
* Particle System
  * [William T. Reeves, particle systems - a technique for modeling a class of fuzzy objects](https://www.lri.fr/~mbl/ENS/IG2/devoir2/files/docs/fuzzyParticles.pdf)
  * [Lutz Lata, Building a Million Particle System](https://citeseerx.ist.psu.edu/document?repid=rep1&type=pdf&doi=b5aa42d88a178b264f4ceb3ddb67d3d00ecbc631)
  * [Karl Sims, Particle animation and rendering using data parallel computation](https://www.karlsims.com/papers/ParticlesSiggraph90.pdf)
  * [GPU based paritlce system - Wicked engine](https://wickedengine.net/2017/11/07/gpu-based-particle-simulation/)
  * DirectX12 Mini Engine

* Parallel sort
  * [Bathcer's Algorithm](https://math.mit.edu/~shor/18.310/batcher.pdf)
  * [Bitonic sorter](https://en.wikipedia.org/wiki/Bitonic_sorter)

