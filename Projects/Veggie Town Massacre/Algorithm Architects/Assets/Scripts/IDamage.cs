using System.Collections;
using System.Collections.Generic;
using UnityEngine;
//
public enum damageType { bullet, chaser, stationary, butter, melee, bouncing, fire, tomato, cabbage, toxic, king, SplitBullet }
public interface IDamage
{
    void takeDamage(int amount, Vector3 dir, damageType type);
}
