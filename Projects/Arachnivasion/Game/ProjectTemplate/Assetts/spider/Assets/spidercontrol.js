#pragma strict





private var SPIDER : GameObject; SPIDER = GameObject.Find("spider1MESH");

var texturenum = 1;
var texture1 : Texture2D;
var texture2 : Texture2D;
var texture3 : Texture2D;
var texture4 : Texture2D;
var texture5 : Texture2D;

function Update () {


	if(Input.GetKeyDown(KeyCode.Space)){
		//print("space key pressed");

			if (texturenum == 1){
				SPIDER.GetComponent(SkinnedMeshRenderer).material.SetTexture("_MainTex", texture2);
				texturenum = 2;
			}
			else if (texturenum == 2){
				SPIDER.GetComponent(SkinnedMeshRenderer).material.SetTexture("_MainTex", texture3);
				texturenum = 3;
			}
			else if (texturenum == 3){
				SPIDER.GetComponent(SkinnedMeshRenderer).material.SetTexture("_MainTex", texture4);
				texturenum = 4;
			}
			else if (texturenum == 4){
				SPIDER.GetComponent(SkinnedMeshRenderer).material.SetTexture("_MainTex", texture5);
				texturenum = 5;
			}
			else if (texturenum == 5){
				SPIDER.GetComponent(SkinnedMeshRenderer).material.SetTexture("_MainTex", texture1);
				texturenum = 1;
			}

	} 
}



