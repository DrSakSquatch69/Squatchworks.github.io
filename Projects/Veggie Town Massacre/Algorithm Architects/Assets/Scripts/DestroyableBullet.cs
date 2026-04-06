using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DestroyableBullet : MonoBehaviour, IDamage
{
    [SerializeField] int HP;
    [SerializeField] Renderer model;
    Color colorOrig;
    //
    // Start is called before the first frame update
    void Start()
    {
        //stores the original color
        colorOrig = model.material.color;
    }

    // Update is called once per frame
    void Update()
    {
        
    }
    public void takeDamage(int amount, Vector3 dir, damageType type)
    {
        HP -= amount;

        StartCoroutine(flashColor());

        //when hp is zero or less, it destroys the object
        if (HP <= 0)
        {
             // Destroys current enemy
             Destroy(gameObject);
        }
    }
    IEnumerator flashColor()
    {
        model.material.color = Color.red;
        yield return new WaitForSeconds(0.1f);
        model.material.color = colorOrig;
    }
}
