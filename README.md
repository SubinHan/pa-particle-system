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
* Unreal Engine 5 Niagara <br>
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

##### ���� ���� Task
* Particle System �н�
* Lifetime, velocity �� GPU �ڵ忡�� �ϵ��ڵ� �� �Ӽ����� CPU �ܿ��� �Ű�����ȭ
* ������ ��� ��� ����
* HLSL Translator ����
  * �߻�ȭ�� �۾����� HLSL �ڵ�� ��ȯ�����ִ� Ŭ����
  * ���� float4 ������ �ʱ�ȭ�ϰ�, �� ���� float4 ������ ���ϴ� �� ��
  * �̸� ������� ������ Emit, Simulate process ������ ��� ����

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
* Bitonic sort�� ���� ����?
  * data access ������ ��Ģ���̶� cache hit�� �� �̷���� �� ����.
<hr/>



### ������
[William T. Reeves, particle systems - a technique for modeling a class of fuzzy objects](https://www.lri.fr/~mbl/ENS/IG2/devoir2/files/docs/fuzzyParticles.pdf) <br>
[Lutz Lata, Building a Million Particle System](https://citeseerx.ist.psu.edu/document?repid=rep1&type=pdf&doi=b5aa42d88a178b264f4ceb3ddb67d3d00ecbc631) <br>
[Karl Sims, Particle animation and rendering using data parallel computation](https://www.karlsims.com/papers/ParticlesSiggraph90.pdf) <br>
[Bathcer's Algorithm](https://math.mit.edu/~shor/18.310/batcher.pdf)
