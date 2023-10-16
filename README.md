## Particle System

### ����
[������Ʈ ����](#������Ʈ-����) <br>
[��ǥ �� ��ȹ](#��ǥ-��-��ȹ) <br>
[Tasks](#Tasks) <br>
[����](#����) 

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
* emitter, particle �� �ٽ� ��� ���� �� ����
* Particle ����/�ı�, �ùķ��̼��� ���� Computing Shader �ۼ�
* Particle System�� ���� ������ ���������� ����

##### �Ϸ�� Tasks
* D3D12 ���� ȯ�� ���� (+PIX �����)
* D3D12 ������ ������ ����

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
  + ParticleSimulateCS ����: ByteAddressBuffer�� InterlockedAdd, Store, Load ��� �ڵ忡 ������ �־� ����ȭ�� ����� ������� ���� - �ذ�: ���̴� �ڵ尡 �ƴ� CPU �ڵ忡 ������ �־���. �޸� �ּҸ� �߸� �Ѱ��־� ���������� ���� ���� �������� �־���.
* 

<hr/>

### ������
[William T. Reeves, particle systems - a technique for modeling a class of fuzzy objects](https://www.lri.fr/~mbl/ENS/IG2/devoir2/files/docs/fuzzyParticles.pdf) <br>
[Lutz Lata, Building a Million Particle System](https://citeseerx.ist.psu.edu/document?repid=rep1&type=pdf&doi=b5aa42d88a178b264f4ceb3ddb67d3d00ecbc631) <br>
[Karl Sims, Particle animation and rendering using data parallel computation](https://www.karlsims.com/papers/ParticlesSiggraph90.pdf)
