//using Microsoft.Unity.VisualStudio.Editor;
using System.Collections;
using System.Collections.Generic;
using UnityEngine.UI;
using UnityEngine;


[CreateAssetMenu]
//
public class gunStats : ScriptableObject
{
    public GameObject gunModel;
    public GameObject prefab;
    public int shootDamage;
    public float shootRate;
    public int shootDist;
    public int magSize;
    public int ammoremaining;
    public int ammo;
    public bool isMelee;
    public bool altModel;
    public Texture icon;
    public Vector3 Muzzleplacement;
    //public Vector3 rotation;

    public ParticleSystem hitEffect;
    public AudioClip shootSound;
}
