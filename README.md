## Particle System

### 목차
[프로젝트 개요](#프로젝트-개요) <br>
[목표 및 계획](#목표-및-계획) <br>
[Tasks](#Tasks) <br>
[일지](#일지) 

<hr/>

### 프로젝트 개요

단순한 수준의 파티클 시스템을 D3D12로 구현합니다. <br>
특히, **Simulation이 아닌 Particle Control에 집중**합니다. <br>


#### 구현 목표 Reference
* Unreal Engine 5 Niagara <br>
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

##### 진행 중인 Task
* Particle System 학습
* emitter, particle 등 핵심 기능 설계 및 구현
* Particle 생성/파괴, 시뮬레이션을 위한 Computing Shader 작성
* Particle System을 위한 렌더링 파이프라인 구성

##### 완료된 Tasks
* D3D12 개발 환경 구성 (+PIX 디버거)
* D3D12 기초적 렌더러 구현

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
  + ParticleSimulateCS 버그: ByteAddressBuffer의 InterlockedAdd, Store, Load 사용 코드에 문제가 있어 동기화가 제대로 수행되지 않음 - 해결: 쉐이더 코드가 아닌 CPU 코드에 문제가 있었음. 메모리 주소를 잘못 넘겨주어 비정상적인 값이 덮어 씌워지고 있었음.
* 

<hr/>

### 참고문헌
[William T. Reeves, particle systems - a technique for modeling a class of fuzzy objects](https://www.lri.fr/~mbl/ENS/IG2/devoir2/files/docs/fuzzyParticles.pdf) <br>
[Lutz Lata, Building a Million Particle System](https://citeseerx.ist.psu.edu/document?repid=rep1&type=pdf&doi=b5aa42d88a178b264f4ceb3ddb67d3d00ecbc631) <br>
[Karl Sims, Particle animation and rendering using data parallel computation](https://www.karlsims.com/papers/ParticlesSiggraph90.pdf)
