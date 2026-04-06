Shader "Custom/SkyboxShader"
{
    Properties
    {
        _TopColor ("Top Color", Color) = (0.1, 0.4, 0.8, 1) // Sky color at the top during day
        _HorizonColor ("Horizon Color", Color) = (1, 0.7, 0.4, 1) // Horizon color during day
        _BottomColor ("Bottom Color", Color) = (0.1, 0.1, 0.2, 1) // Bottom of sky during day
        _NightColor ("Night Color", Color) = (0, 0, 0.05, 1) // Dark sky color for night
        _BlendFactor ("Blend Factor", Range(0,1)) = 0.5 // For day-to-night transition
    }

    SubShader
    {
        Tags { "Queue" = "Background" "RenderType" = "Opaque" }
        Pass
        {
            CGPROGRAM
            #pragma vertex vert
            #pragma fragment frag

            struct appdata
            {
                float4 vertex : POSITION;
            };

            struct v2f
            {
                float4 pos : SV_POSITION;
                float3 worldPos : TEXCOORD0;
            };

            sampler2D _MainTex;
            float4 _TopColor;
            float4 _HorizonColor;
            float4 _BottomColor;
            float4 _NightColor;
            float _BlendFactor;

            v2f vert (appdata v)
            {
                v2f o;
                o.pos = UnityObjectToClipPos(v.vertex);
                o.worldPos = normalize(v.vertex.xyz);
                return o;
            }

            fixed4 frag (v2f i) : SV_Target
            {
                // Calculate height gradient based on y position
                float height = i.worldPos.y;

                // Interpolate day colors
                float4 dayColor = lerp(_BottomColor, _HorizonColor, smoothstep(-0.2, 0.2, height));
                dayColor = lerp(dayColor, _TopColor, smoothstep(0.2, 1.0, height));

                // Transition to night color based on BlendFactor
                float4 finalColor = lerp(_NightColor, dayColor, 1 - _BlendFactor);

                return finalColor;
            }
            ENDCG
        }
    }
}
