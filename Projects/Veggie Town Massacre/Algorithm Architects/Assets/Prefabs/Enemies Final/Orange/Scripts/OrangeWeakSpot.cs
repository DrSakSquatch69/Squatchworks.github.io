using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class OrangeWeakSpot : MonoBehaviour, IDamage
{
    [SerializeField] OrangeAI ParentEnemy;
    private List<Renderer> ParentBody;
    void Start()
    {
        ParentBody = ParentEnemy.Body;    
    }
    public void takeDamage(int amount, Vector3 dir, damageType type)
    {
        //Debug.Log("hit weak spot");
        ParentEnemy.Damage(amount);
        Color origColor;
        for (int i = 0; i < ParentBody.Count - 1; i++)
        {
            origColor = ParentBody[i].material.color;
            StartCoroutine(flashColor(ParentBody[i], origColor));
                     
        }
    }
    void DamageParent(int amount, Vector3 dir, damageType type, List<Renderer> models)
    {

    }
    IEnumerator flashColor(Renderer model, Color origColor)
    {
        model.material.color = Color.red;
        yield return new WaitForSeconds(0.1f);
        model.material.color = origColor;
    }
}
