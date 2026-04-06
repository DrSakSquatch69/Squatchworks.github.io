Shader "Custom/SkyboxShader"
{
    Properties
    {
        _TopColor ("Top Color", Color) = (0.1, 0.4, 0.8, 1) // Sky Color at the top (e.g., blue)
        _HorizonColor ("Horizon Color", Color) = (1, 0.7, 0.4, 1) // Color at the horizon (e.g., warm orange)
        _BottomColor ("Bottom Color", Color) = (0.1, 0.1, 0.2, 1) // Color below the horizon (e.g., dark purple)
        _NightColor ("Night Color", Color) = (0, 0, 0.05, 1) // Dark sky color for night
        _BlendFactor ("Blend Factor", Range(0,.7)) = 0.5 // Used to transition between day and night
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
                // Calculate vertical gradient based on y position of the world position
                float height = i.worldPos.y;

                // Interpolate between bottom, horizon, and top color
                float4 skyColor = lerp(_BottomColor, _HorizonColor, smoothstep(-0.2, 0.2, height));
                skyColor = lerp(skyColor, _TopColor, smoothstep(0.2, 1.0, height));

                // Blend factor for day-night transition
                skyColor = lerp(skyColor, float4(0, 0, 0, 1), _BlendFactor);

                return skyColor;
            }
            ENDCG
        }
    }
}