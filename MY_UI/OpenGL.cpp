#include "OpenGL.h"
#include <math.h>
#include <vector>
#include <fstream>
#include "cWidgetSkin.h"

const std::string VS_UIShader = std::string(
	"#version 150\n"
	"in vec2 position;\n"
	"in vec2 texcoord;\n"
	"in vec4 incolor;\n"
	"out vec4 color;\n"
	"out vec2 UV;\n"
	"void main() { \n"
		"gl_Position = vec4(position.xy, 1, 1);\n"
		"color = vec4(incolor);\n"
		"UV = vec2(texcoord.x, texcoord.y-1);\n"
	"}\n"
);
const std::string PS_UIShader = std::string(
	"#version 150\n"
	"in vec4 color;\n"
	"in vec2 UV;\n"
	"out vec4 outputColor;\n"
	"uniform sampler2D UITexture;\n"
	"void main(){\n"
	"	outputColor =  texture( UITexture, UV ) * color;\n"
	"}\n"
);


GLuint InitShader(std::string vShaderFile, std::string fShaderFile)
{
	GLuint program = glCreateProgram();

	GLuint shader = glCreateShader(GL_VERTEX_SHADER );
	const char* ptr = vShaderFile.c_str();
	glShaderSource( shader, 1, (const GLchar**) &ptr, NULL );
	glCompileShader( shader );

	GLint  compiled;
	glGetShaderiv( shader, GL_COMPILE_STATUS, &compiled );
	if ( !compiled ) {
		GLint  logSize;
		glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &logSize );
		char* logMsg = new char[logSize];
		glGetShaderInfoLog( shader, logSize, NULL, logMsg );
		std::string tempm = logMsg;
		OUTPUT_DEBUG_MSG("Failed to Compiler the Vertex Shader. . . \n" + tempm);
		delete [] logMsg;
	}
	glAttachShader( program, shader );
	ptr = fShaderFile.c_str();
	shader = glCreateShader(GL_FRAGMENT_SHADER );
	glShaderSource( shader, 1, (const GLchar**) &ptr, NULL );
	glCompileShader( shader );

	glGetShaderiv( shader, GL_COMPILE_STATUS, &compiled );
	if ( !compiled ) {
		GLint  logSize;
		glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &logSize );
		char* logMsg = new char[logSize];
		glGetShaderInfoLog( shader, logSize, NULL, logMsg );
		std::string tempm = logMsg;
		OUTPUT_DEBUG_MSG("Failed to Compiler the Pixel Shader. . . \n" + tempm);
		delete [] logMsg;
	}
	glAttachShader( program, shader );
	glLinkProgram(program);

	GLint  linked;
	glGetProgramiv( program, GL_LINK_STATUS, &linked );
	if ( !linked ) {
		std::cerr << "Shader program failed to link" << std::endl;
		GLint  logSize;
		glGetProgramiv( program, GL_INFO_LOG_LENGTH, &logSize);
		char* logMsg = new char[logSize];
		glGetProgramInfoLog( program, logSize, NULL, logMsg );
		std::string tempm = logMsg;
		OUTPUT_DEBUG_MSG("Shader program failed to link. . . \n " + tempm);
		delete [] logMsg;
	}

	glUseProgram(program);
	return program;
}

MY_UI::OpenGL_Renderer::OpenGL_Renderer(){

	Draw_State_Index=0;
}

MY_UI::OpenGL_Renderer::~OpenGL_Renderer(){		
	DeInit();

}

bool MY_UI::OpenGL_Renderer::Init(){
	::FreeImage_Initialise();
	GL_Shader = InitShader(VS_UIShader, PS_UIShader);
	glGenBuffers( 1, &Vertex_Buffer );
	glBindBuffer( GL_ARRAY_BUFFER, Vertex_Buffer );
	glBufferData( GL_ARRAY_BUFFER, sizeof(VERTEXFORMAT2D)* VERT_BUFFER_SIZE, NULL, GL_DYNAMIC_DRAW );

	std::vector<unsigned int> indexbuffer;
	unsigned int currindex =0;
	indexbuffer.resize((VERT_BUFFER_SIZE/4)*6 );
	for(unsigned int i=0; i<indexbuffer.size(); i+=6){
		indexbuffer[i + 0] = currindex;
		indexbuffer[i + 1] = currindex + 1;
		indexbuffer[i + 2] = currindex + 2;
		indexbuffer[i + 3] = currindex + 1;
		indexbuffer[i + 4] = currindex + 3;
		indexbuffer[i + 5] = currindex + 2;
		currindex+=4;
	}

	glGenBuffers( 1, &Index_Buffer );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, Index_Buffer );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indexbuffer.size(), &indexbuffer[0], GL_STATIC_DRAW );

	texture_loc = glGetUniformLocation(GL_Shader, "UITexture");

	pos_loc = glGetAttribLocation( GL_Shader, "position" );
	color_loc = glGetAttribLocation( GL_Shader, "incolor" );
	tex_loc = glGetAttribLocation( GL_Shader, "texcoord" );

	Draw_States.resize(1);// at least one
	Draw_State_Index=0;
	glClearColor(0, 0, 1, 0);
	return true;
}
void MY_UI::OpenGL_Renderer::DeInit(){

	::FreeImage_DeInitialise();

}

void MY_UI::OpenGL_Renderer::Begin()// get the states that this will change to set at the end call
{
	glUseProgram( GL_Shader );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glAlphaFunc( GL_GREATER, 1.0f );	
	glEnable ( GL_BLEND );
	DrawCalls =0;

}

void MY_UI::OpenGL_Renderer::End(){// reset the device to its original state{

}

void MY_UI::OpenGL_Renderer::DrawBufferd(){	
	for(size_t i(0); i < Draw_State_Index +1; i++){
		if(Draw_States[i].NumVerts == 0) continue;
		Draw(Draw_States[i]);
	}
}
void MY_UI::OpenGL_Renderer::DrawBack(bool popback){
	Draw(Draw_States[ Draw_State_Index ]);
	if(popback){
		Draw_States[ Draw_State_Index ].NumVerts=0;
		Draw_State_Index -= 1;
		CurrentTexture = Draw_States[ Draw_State_Index ].texture;//reset this
	}
}
void MY_UI::OpenGL_Renderer::Draw(const MyDrawState& drawstate){
	++DrawCalls;

	glBindBuffer( GL_ARRAY_BUFFER, Vertex_Buffer );
	glBufferSubData( GL_ARRAY_BUFFER, 0, drawstate.NumVerts * sizeof( VERTEXFORMAT2D ) , &drawstate.Verts[0]);

	glEnableVertexAttribArray( pos_loc );
	glEnableVertexAttribArray( color_loc );
	glEnableVertexAttribArray( tex_loc );

	glVertexAttribPointer( pos_loc, 2, GL_FLOAT, GL_FALSE, sizeof( VERTEXFORMAT2D ), BUFFER_OFFSET(0) );
	glVertexAttribPointer( color_loc, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof( VERTEXFORMAT2D ), BUFFER_OFFSET(sizeof(float)*2) );
	glVertexAttribPointer( tex_loc, 2, GL_FLOAT, GL_FALSE, sizeof( VERTEXFORMAT2D ), BUFFER_OFFSET(sizeof(unsigned int) + (sizeof(float) *2) ) );
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(texture_loc, 0);
	glBindTexture( GL_TEXTURE_2D, *(GLuint*)drawstate.texture);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Index_Buffer);
	glDrawElements(GL_TRIANGLES, (drawstate.NumVerts/4)*6, GL_UNSIGNED_INT, 0);	

}
void MY_UI::OpenGL_Renderer::GotoNextBufferSlot(){		
	if(Draw_States.size() <= Draw_State_Index +1)  Draw_States.resize(Draw_States.size() + 16);// add more space if needed
	Draw_States[ Draw_State_Index ].Verts.resize(Draw_States[ Draw_State_Index ].NumVerts);// resize the buffer in case there is hardly any data inside
	Draw_State_Index+=1;// goto the next index
}
void MY_UI::OpenGL_Renderer::AddVert( float x, float y, float u, float v, Utilities::Color col){
	if ( Draw_States[ Draw_State_Index ].NumVerts >= VERT_BUFFER_SIZE) GotoNextBufferSlot();

	MyDrawState& drawstate = Draw_States[ Draw_State_Index ];
	VERTEXFORMAT2D& vert = drawstate.Verts[drawstate.NumVerts];
	vert.x = (x*Inv_Wndx) -1.0f;
	vert.y = (y*Inv_Wndy) +1.0f;
	vert.u = u;
	vert.v = v;	
	vert.color = col.color;
	drawstate.NumVerts+=1;
}
bool MY_UI::OpenGL_Renderer::SetTexture(MY_UI::Utilities::Texture& pTexture, bool drawingnow){
	void* pImage = pTexture.Tex;
	if ( pImage ==0) return false;// Missing image, not loaded properly?
	if( (CurrentTexture!= pImage) | (drawingnow)){
		GotoNextBufferSlot();
	}
	Draw_States[ Draw_State_Index ].texture = pImage;
	CurrentTexture =pImage;

	return true;
}

void MY_UI::OpenGL_Renderer::DrawTexturedRect_Clip(MY_UI::Utilities::Texture& pTexture, MY_UI::Utilities::UVs& uvs, MY_UI::Utilities::Rect rect,  MY_UI::Utilities::Color color_tl, MY_UI::Utilities::Color color_tr, MY_UI::Utilities::Color color_bl, MY_UI::Utilities::Color color_br, bool drawnow){
	if(!SetTexture(pTexture, drawnow)) return;

	Utilities::Point tl(rect.left, rect.top);
	Utilities::Point tr(rect.left + rect.width, rect.top);
	Utilities::Point bl(rect.left, rect.top + rect.height);
	Utilities::Point br(rect.left + rect.width, rect.top + rect.height);
	// if all the points are not within the cliprect, dont draw it
	bool brin = ClipRects.back().Within(br);
	bool trin = ClipRects.back().Within(tr);

	bool blin = ClipRects.back().Within(bl);
	bool tlin = ClipRects.back().Within(tl);

	if( (!brin) & (!trin) & (!blin) & (!tlin)) return;// all points are outside the cliprect


	float left= static_cast<float>(rect.left);
	float top = static_cast<float>(rect.top);
	float width = static_cast<float>(rect.width);
	float right = width + left;
	float height = static_cast<float>(rect.height);
	float bottom = height + top;
	// resize the buffer if needed
	if(Draw_States[ Draw_State_Index ].Verts.size() <= 4 + Draw_States[ Draw_State_Index ].NumVerts ) Draw_States[ Draw_State_Index ].Verts.resize(Draw_States[ Draw_State_Index ].Verts.size() + 200);
	if( (brin) & (trin) & (blin) & (tlin)){// all points are fully contained inside the cliprect

		AddVert( left, top,					uvs.u1, uvs.v1, color_tl );
		AddVert( right, top,				uvs.u2, uvs.v1, color_tr );
		AddVert( left, bottom,				uvs.u1, uvs.v2, color_bl );
		AddVert( right, bottom,				uvs.u2, uvs.v2, color_br );


	} else {// this means the rect is partially in the clip region. Use the cpu to clip it

		Utilities::Rect& r = ClipRects.back();
		float newleft= static_cast<float>(Utilities::Clamp<int>(rect.left, r.left, r.left + r.width));
		float newtop = static_cast<float>(Utilities::Clamp<int>(rect.top, r.top, r.top + r.height));
		float newright = static_cast<float>(Utilities::Clamp<int>(rect.width + rect.left, r.left, r.left + r.width));
		float newbottom = static_cast<float>(Utilities::Clamp<int>(rect.height + rect.top, r.top, r.top + r.height));

		float difleft = newleft - left;
		float diftop = newtop - top;
		float difright = newright - right;
		float difbottom = newbottom - bottom;

		difleft /= width;
		diftop /= height;
		difright /= width;
		difbottom /= height;

		float u1 = uvs.u1;
		float v1 = uvs.v1;
		float u2 = uvs.u2;
		float v2 = uvs.v2;

		float uwidth = u2 - u1;
		float vheight = v2 - v1;

		u1 = u1 + (uwidth*difleft);
		u2 = u2 + (uwidth*difright);
		v1 = v1 + (vheight*diftop);
		v2 = v2 + (vheight*difbottom);

		AddVert( newleft, newtop,					u1, v1, color_tl );
		AddVert( newright, newtop,					u2, v1, color_tr );
		AddVert( newleft, newbottom,				u1, v2, color_bl );
		AddVert( newright, newbottom,				u2, v2, color_br );

	}
}

void MY_UI::OpenGL_Renderer::DrawTexturedRect_NoClip(MY_UI::Utilities::Texture& pTexture,  MY_UI::Utilities::UVs& uvs, MY_UI::Utilities::Rect rect,  MY_UI::Utilities::Color color_tl, MY_UI::Utilities::Color color_tr, MY_UI::Utilities::Color color_bl, MY_UI::Utilities::Color color_br, bool drawnow){
	if(!SetTexture(pTexture, drawnow)) return;

	float left= static_cast<float>(rect.left);
	float top = static_cast<float>(rect.top);
	float width = static_cast<float>(rect.width);
	float right = width + left;
	float height = static_cast<float>(rect.height);
	float bottom = height + top;
	if(Draw_States[ Draw_State_Index ].Verts.size() <= 4 + Draw_States[ Draw_State_Index ].NumVerts ) Draw_States[ Draw_State_Index ].Verts.resize(Draw_States[ Draw_State_Index ].Verts.size() + 200);
	AddVert( left, top,					uvs.u1, uvs.v1, color_tl );
	AddVert( right, top,				uvs.u2, uvs.v1, color_tr );
	AddVert( left, bottom,				uvs.u1, uvs.v2, color_bl );
	AddVert( right, bottom,				uvs.u2, uvs.v2, color_br );
}
void MY_UI::OpenGL_Renderer::DrawLine_Clip(MY_UI::Utilities::Texture& pTexture, MY_UI::Utilities::Point beg, MY_UI::Utilities::Point end, int thickness, MY_UI::Utilities::Color color_beg, MY_UI::Utilities::Color color_end, bool drawnow){
	// havent implemented the clip version yet. . . .
	DrawLine_NoClip(pTexture, beg, end, thickness, color_beg, color_end, drawnow);
}

void MY_UI::OpenGL_Renderer::DrawLine_NoClip(MY_UI::Utilities::Texture& pTexture, MY_UI::Utilities::Point beg, MY_UI::Utilities::Point end, int thickness, MY_UI::Utilities::Color color_beg, MY_UI::Utilities::Color color_end, bool drawnow){
	if(!SetTexture(pTexture, drawnow)) return;

	float beg_x= static_cast<float>(beg.x);
	float beg_y = static_cast<float>(beg.y);
	float end_x = static_cast<float>(end.x);
	float end_y = static_cast<float>(end.y);

	float otherx = end_x-beg_x;
	float othery = end_y-beg_y;

	// now find the normal, make sure to use the largest as the direction, but negaive
	if(othery > otherx) othery = -othery;
	else otherx = -otherx;

	//other is now the vector between beg and other
	float inv_mag = otherx*otherx + othery*othery;
	// no check for div by zero here!
	inv_mag = 1.0f/sqrt(inv_mag);
	otherx*=inv_mag;
	othery*=inv_mag;
	// the components must be swapped 
	float temp = otherx;
	otherx = othery;
	othery = temp;

	//other is now the normalized vector pointing perpendicular to both beg and end
	thickness>>=1;// divide by two since thickness is the total thickness
	float thick = static_cast<float>(thickness);
	float topleft_x = otherx*thick + beg_x;
	float topleft_y = othery*thick + beg_y;

	float botleft_x = -otherx*thick + beg_x;
	float botleft_y = -othery*thick + beg_y;

	float topright_x = otherx*thick + end_x;
	float topright_y = othery*thick + end_y;

	float botright_x = -otherx*thick + end_x;
	float botright_y = -othery*thick + end_y;
	if(Draw_States[ Draw_State_Index ].Verts.size() <= 4 + Draw_States[ Draw_State_Index ].NumVerts ) Draw_States[ Draw_State_Index ].Verts.resize(Draw_States[ Draw_State_Index ].Verts.size() + 200);

	AddVert( topleft_x, topleft_y,		pTexture.u1, pTexture.v1, color_beg );
	AddVert( topright_x, topright_y,	pTexture.u2, pTexture.v1, color_end );
	AddVert( botleft_x, botleft_y,		pTexture.u1, pTexture.v2, color_beg );
	AddVert( botright_x, botright_y,	pTexture.u2, pTexture.v2, color_end );

}


void MY_UI::OpenGL_Renderer::StartClip(MY_UI::Utilities::Rect& rect){
	ClipRects.push_back(rect);
}

void MY_UI::OpenGL_Renderer::EndClip(){
	ClipRects.pop_back();
}

bool MY_UI::OpenGL_Renderer::LoadTexture( MY_UI::Utilities::Texture* pTexture ){	
	FREE_IMAGE_FORMAT imageFormat = FreeImage_GetFileType( pTexture->FileName.c_str() );
	if ( imageFormat == FIF_UNKNOWN ) imageFormat = FreeImage_GetFIFFromFilename( pTexture->FileName.c_str() );
	// Image failed to load..
	if ( imageFormat == FIF_UNKNOWN ) {
		OUTPUT_DEBUG_MSG("Could not Load image IFF");
		return false;
	}

	// Try to load the image..
	FIBITMAP* bits = FreeImage_Load( imageFormat, pTexture->FileName.c_str() );
	if ( !bits ) {
		OUTPUT_DEBUG_MSG("Could not Load image");
		return false;
	}
	// Convert to 32bit
	FIBITMAP * bits32 = FreeImage_ConvertTo32Bits( bits );
	FreeImage_Unload( bits );
	if ( !bits32 ) {
		OUTPUT_DEBUG_MSG("Could not convert to 32 bits");
		return false;
	}

	// Flip
	::FreeImage_FlipVertical( bits32 );

	// Create a little texture pointer..
	GLuint* pglTexture = new GLuint;
	pTexture->Tex = pglTexture;
	pTexture->width = (float)FreeImage_GetWidth( bits32 );
	pTexture->height = (float)FreeImage_GetHeight( bits32 );

	// Create the opengl texture
	glActiveTexture(GL_TEXTURE0);
	glGenTextures( 1, pglTexture );
	glBindTexture( GL_TEXTURE_2D, *pglTexture );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

#ifdef FREEIMAGE_BIGENDIAN
	GLenum format = GL_RGBA;
#else
	GLenum format = GL_BGRA;
#endif

	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)pTexture->width, (GLsizei)pTexture->height, 0, format, GL_UNSIGNED_BYTE, (const GLvoid*)FreeImage_GetBits( bits32 ) );
	FreeImage_Unload( bits32 );
	return true;
}

void MY_UI::OpenGL_Renderer::FreeTexture( MY_UI::Utilities::Texture* pTexture ){			
	GLuint* tex = (GLuint*)pTexture->Tex;
	if ( !tex ) return;

	glDeleteTextures( 1, tex );
	delete tex;
	pTexture->Tex = NULL;
}
