#ifndef TERRAIN_H
#define TERRAIN_H
#include "../Graphics_Lib/GraphicsCore.h"
#include <vector>
#include <mutex>
#include <atomic>
#include "../Common_Game_Objects_Lib/Base_Terrain.h"

const std::string DiffuseTextures[] = {
	"Ground Textures/brumacobblestone01.dds",
	"Ground Textures/chterraincliff01.dds",
	"Ground Textures/chterraindirt01.dds",
	"Ground Textures/chterraindirtgrass01.dds",
	"Ground Textures/chterraingrass01.dds",
	"Ground Textures/chterrainmoss01.dds",
	"Ground Textures/chterrainrockydirt01.dds",
	"Ground Textures/chterrainrockymoss01.dds",
	"Ground Textures/cobblestone01.dds",
	"Ground Textures/cobblestone02.dds",
	"Ground Textures/cobblestone03.dds",
	"Ground Textures/cobblestone04.dds",
	"Ground Textures/cobblestonecheydinhal01.dds",
	"Ground Textures/cobblestonecheydinhal02.dds",
	"Ground Textures/cobblestonecheydinhal03.dds",
	"Ground Textures/cobblestonecheydinhal04.dds",
	"Ground Textures/cobblestonecheydinhal05.dds",
	"Ground Textures/cobblestoneleyawiin01.dds",
	"Ground Textures/cobblestoneleyawiin02.dds",
	"Ground Textures/cobblestoneleyawiin03.dds",
	"Ground Textures/default.dds",
	"Ground Textures/dustystone.dds",
	"Ground Textures/greatforestrock01.dds",
	"Ground Textures/greatforestrockmoss02.dds",
	"Ground Textures/greatforestrockmoss03.dds",
	"Ground Textures/jmpath01.dds",
	"Ground Textures/jmrock01.dds",
	"Ground Textures/jmrock02.dds",
	"Ground Textures/jmrocksnow03.dds",
	"Ground Textures/jmstoneground.dds",
	"Ground Textures/roadstone01.dds",
	"Ground Textures/roadstonesmall01.dds",
	"Ground Textures/terrainanvilsand01.dds",
	"Ground Textures/terrainanvilstreet.dds",
	"Ground Textures/terrainanvilstreet02.dds",
	"Ground Textures/terrainanvilstreet03.dds",
	"Ground Textures/terrainbravilbase01.dds",
	"Ground Textures/terrainbravilbase02.dds",
	"Ground Textures/terrainbravilbase03.dds",
	"Ground Textures/terrainbravilbase04.dds",
	"Ground Textures/terrainbravilbaseroad01.dds",
	"Ground Textures/terrainbravilbaseroad02.dds",
	"Ground Textures/terrainbrustone.dds",
	"Ground Textures/terrainburnedground01.dds",
	"Ground Textures/terrainburnedground02.dds",
	"Ground Textures/terrainburnedground03.dds",
	"Ground Textures/terrainburnedground04.dds",
	"Ground Textures/terrainburnedrubble01.dds",
	"Ground Textures/terrainburnedrubblesmolder.dds",
	"Ground Textures/terrainbwlichenmud.dds",
	"Ground Textures/terrainbwmoss.dds",
	"Ground Textures/terrainbwmossdirt.dds",
	"Ground Textures/terrainbwmossgrass.dds",
	"Ground Textures/terrainbwmudmoss.dds",
	"Ground Textures/terrainbwsandmoss.dds",
	"Ground Textures/terrainbwsandmossrocks.dds",
	"Ground Textures/terrainbwtreerootsmoss.dds",
	"Ground Textures/terraincplichenmud.dds",
	"Ground Textures/terraincpmoss.dds",
	"Ground Textures/terraincpmossdirt.dds",
	"Ground Textures/terraincpmossgrass.dds",
	"Ground Textures/terraincpmudmoss.dds",
	"Ground Textures/terraincpsandmoss.dds",
	"Ground Textures/terraincpsandmossrocks.dds",
	"Ground Textures/terraincptreerootsmoss.dds",
	"Ground Textures/terraingcgrass01.dds",
	"Ground Textures/terraingcgrassdirt01.dds",
	"Ground Textures/terrainhdclover01su.dds",
	"Ground Textures/terrainhddeadgrass01fa.dds",
	"Ground Textures/terrainhddeadleaves01.dds",
	"Ground Textures/terrainhddirt01.dds",
	"Ground Textures/terrainhdevergreenneedles01.dds",
	"Ground Textures/terrainhdforestdirt01.dds",
	"Ground Textures/terrainhdforestgrassdirtrocks01su.dds",
	"Ground Textures/terrainhdforestgrassleaves01su.dds",
	"Ground Textures/terrainhdforestgrasstwigsdirt01su.dds",
	"Ground Textures/terrainhdforestmossgrassdirt01su.dds",
	"Ground Textures/terrainhdgrass01su.dds",
	"Ground Textures/terrainhdgrassclover01su.dds",
	"Ground Textures/terrainhdgrassmoss01su.dds",
	"Ground Textures/terrainhdoblivionlavarockburning.dds",
	"Ground Textures/terrainhdrock01.dds",
	"Ground Textures/terrainhdrock02.dds",
	"Ground Textures/terrainhdrock03.dds",
	"Ground Textures/terrainhdrocklichen01.dds",
	"Ground Textures/terrainhdrocklichen02.dds",
	"Ground Textures/terrainhdrocklichen03.dds",
	"Ground Textures/terrainhdrockmoss01.dds",
	"Ground Textures/terrainhdrockmoss02.dds",
	"Ground Textures/terrainhdrockmoss03.dds",
	"Ground Textures/terrainhdrocksdirtmoss01su.dds",
	"Ground Textures/terrainjerallmountstone.dds",
	"Ground Textures/terrainjmgrassdirt.dds",
	"Ground Textures/terrainjmgrassdirt01.dds",
	"Ground Textures/terrainjmgrassdirtsnow.dds",
	"Ground Textures/terrainleyawiindustystone.dds",
	"Ground Textures/terrainmud01.dds",
	"Ground Textures/terrainmud02.dds",
	"Ground Textures/terrainmud03.dds",
	"Ground Textures/terrainmudstone01.dds",
	"Ground Textures/terrainoblivioncooledlava.dds",
	"Ground Textures/terrainskstoneground.dds",
	"Ground Textures/terrainsnow01.dds",
	"Ground Textures/terrainsnowstone01.dds",
	"Ground Textures/terraintilledsoil.dds",
	"Ground Textures/terrainwetsand01.dds",
	"Ground Textures/terrainwetsand02.dds",
	"Ground Textures/terrainwetsand03.dds",
	"Ground Textures/terrainwetsand04.dds",
};

const std::string NormalTextures[] = { 
	"Ground Textures/brumacobblestone01_n.dds",
	"Ground Textures/chterraincliff01_n.dds",
	"Ground Textures/chterraindirt01_n.dds",
	"Ground Textures/chterraindirtgrass01_n.dds",
	"Ground Textures/chterraingrass01_n.dds",
	"Ground Textures/chterrainmoss01_n.dds",
	"Ground Textures/chterrainrockydirt01_n.dds",
	"Ground Textures/chterrainrockymoss01_n.dds",
	"Ground Textures/cobblestone01_n.dds",
	"Ground Textures/cobblestone02_n.dds",
	"Ground Textures/cobblestone03_n.dds",
	"Ground Textures/cobblestone04_n.dds",
	"Ground Textures/cobblestonecheydinhal01_n.dds",
	"Ground Textures/cobblestonecheydinhal02_n.dds",
	"Ground Textures/cobblestonecheydinhal03_n.dds",
	"Ground Textures/cobblestonecheydinhal04_n.dds",
	"Ground Textures/cobblestonecheydinhal05_n.dds",
	"Ground Textures/cobblestoneleyawiin01_n.dds",
	"Ground Textures/cobblestoneleyawiin02_n.dds",
	"Ground Textures/cobblestoneleyawiin03_n.dds",
	"Ground Textures/default_n.dds",
	"Ground Textures/dustystone_n.dds",
	"Ground Textures/greatforestrock01_n.dds",
	"Ground Textures/greatforestrockmoss02_n.dds",
	"Ground Textures/greatforestrockmoss03_n.dds",
	"Ground Textures/jmpath01_n.dds",
	"Ground Textures/jmrock01_n.dds",
	"Ground Textures/jmrock02_n.dds",
	"Ground Textures/jmrocksnow03_n.dds",
	"Ground Textures/jmstoneground_n.dds",
	"Ground Textures/roadstone01_n.dds",
	"Ground Textures/roadstonesmall01_n.dds",
	"Ground Textures/terrainanvilsand01_n.dds",
	"Ground Textures/terrainanvilstreet_n.dds",
	"Ground Textures/terrainanvilstreet02_n.dds",
	"Ground Textures/terrainanvilstreet03_n.dds",
	"Ground Textures/terrainbravilbase01_n.dds",
	"Ground Textures/terrainbravilbase02_n.dds",
	"Ground Textures/terrainbravilbase03_n.dds",
	"Ground Textures/terrainbravilbase04_n.dds",
	"Ground Textures/terrainbravilbaseroad01_n.dds",
	"Ground Textures/terrainbravilbaseroad02_n.dds",
	"Ground Textures/terrainbrustone_n.dds",
	"Ground Textures/terrainburnedground01_n.dds",
	"Ground Textures/terrainburnedground02_n.dds",
	"Ground Textures/terrainburnedground03_n.dds",
	"Ground Textures/terrainburnedground04_n.dds",
	"Ground Textures/terrainburnedrubble01_n.dds",
	"Ground Textures/terrainburnedrubblesmolder_n.dds",
	"Ground Textures/terrainbwlichenmud_n.dds",
	"Ground Textures/terrainbwmoss_n.dds",
	"Ground Textures/terrainbwmossdirt_n.dds",
	"Ground Textures/terrainbwmossgrass_n.dds",
	"Ground Textures/terrainbwmudmoss_n.dds",
	"Ground Textures/terrainbwsandmoss_n.dds",
	"Ground Textures/terrainbwsandmossrocks_n.dds",
	"Ground Textures/terrainbwtreerootsmoss_n.dds",
	"Ground Textures/terraincplichenmud_n.dds",
	"Ground Textures/terraincpmoss_n.dds",
	"Ground Textures/terraincpmossdirt_n.dds",
	"Ground Textures/terraincpmossgrass_n.dds",
	"Ground Textures/terraincpmudmoss_n.dds",
	"Ground Textures/terraincpsandmoss_n.dds",
	"Ground Textures/terraincpsandmossrocks_n.dds",
	"Ground Textures/terraincptreerootsmoss_n.dds",
	"Ground Textures/terraingcgrass01_n.dds",
	"Ground Textures/terraingcgrassdirt01_n.dds",
	"Ground Textures/terrainhdclover01su_n.dds",
	"Ground Textures/terrainhddeadgrass01fa_n.dds",
	"Ground Textures/terrainhddeadleaves01_n.dds",
	"Ground Textures/terrainhddirt01_n.dds",
	"Ground Textures/terrainhdevergreenneedles01_n.dds",
	"Ground Textures/terrainhdforestdirt01_n.dds",
	"Ground Textures/terrainhdforestgrassdirtrocks01su_n.dds",
	"Ground Textures/terrainhdforestgrassleaves01su_n.dds",
	"Ground Textures/terrainhdforestgrasstwigsdirt01su_n.dds",
	"Ground Textures/terrainhdforestmossgrassdirt01su_n.dds",
	"Ground Textures/terrainhdgrass01su_n.dds",
	"Ground Textures/terrainhdgrassclover01su_n.dds",
	"Ground Textures/terrainhdgrassmoss01su_n.dds",
	"Ground Textures/terrainhdoblivionlavarockburning_n.dds",
	"Ground Textures/terrainhdrock01_n.dds",
	"Ground Textures/terrainhdrock02_n.dds",
	"Ground Textures/terrainhdrock03_n.dds",
	"Ground Textures/terrainhdrocklichen01_n.dds",
	"Ground Textures/terrainhdrocklichen02_n.dds",
	"Ground Textures/terrainhdrocklichen03_n.dds",
	"Ground Textures/terrainhdrockmoss01_n.dds",
	"Ground Textures/terrainhdrockmoss02_n.dds",
	"Ground Textures/terrainhdrockmoss03_n.dds",
	"Ground Textures/terrainhdrocksdirtmoss01su_n.dds",
	"Ground Textures/terrainjerallmountstone_n.dds",
	"Ground Textures/terrainjmgrassdirt_n.dds",
	"Ground Textures/terrainjmgrassdirt01_n.dds",
	"Ground Textures/terrainjmgrassdirtsnow_n.dds",
	"Ground Textures/terrainleyawiindustystone_n.dds",
	"Ground Textures/terrainmud01_n.dds",
	"Ground Textures/terrainmud02_n.dds",
	"Ground Textures/terrainmud03_n.dds",
	"Ground Textures/terrainmudstone01_n.dds",
	"Ground Textures/terrainoblivioncooledlava_n.dds",
	"Ground Textures/terrainskstoneground_n.dds",
	"Ground Textures/terrainsnow01_n.dds",
	"Ground Textures/terrainsnowstone01_n.dds",
	"Ground Textures/terraintilledsoil_n.dds",
	"Ground Textures/terrainwetsand01_n.dds",
	"Ground Textures/terrainwetsand02_n.dds",
	"Ground Textures/terrainwetsand03_n.dds",
	"Ground Textures/terrainwetsand04_n.dds",

};

class Simple_Terrain: public Base_Terrain{
public:

	std::vector<vec4> Grids;
	std::vector<vec2> Verticies, TexCoords;
	std::vector<unsigned int> Indicies;

	Graphics::Texture Buffer_NormalHeight, Buffer_Tangents, Buffer_Uvs, Diffuse, Normals;
	Graphics::Texture Staging_Buffer_NormalHeight, Staging_Buffer_Tangents, Staging_Buffer_Uvs;
	Graphics::Buffer Points;
	Graphics::HullShader HS;
	Graphics::DomainShader DS, DS_Editor;
	Graphics::PixelShader PS;
	Graphics::Buffer CBuffer0, CBuffer1;

	vec3 StartingPos;
	enum EditorFuncs{Off, Raise, Lower, Roughen, Smooth, Texture};
	bool Editing, DrawWireFrame;
	vec4 EditorPos;
	vec4 EditorColor;
	float EditorSize;
	unsigned int SelectedTex;
	unsigned int EditorFunction;

	Simple_Terrain(){}
	virtual ~Simple_Terrain(){ DeInit(); }

	void Init();
	void DeInit();

	void Draw();
	void Update();

	void BeginBuildingNext();

	void InitTempGrid();
	void DeInitTempGrid();

	void InitGrids();
	void DeInitGrids();

	void InitTextures();
	void InitTextures(unsigned int row, unsigned int col);

	void DeInitTextures();


};

#endif