using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class StrawberryBullet : MonoBehaviour
{
    [Header("Bullet Settings")]
    [SerializeField] int burstBulletCount = 200;    // Number of bullets in the burst
    [SerializeField] int bulletDamage = 10;         // Damage dealt by each bullet
    [SerializeField] float bulletSpeed = 20f;       // Speed of each bullet
    [SerializeField] float despawnTimer = 5f;       // Time before each bullet is destroyed

    [Header("Burst Settings")]
    [SerializeField] float burstDelay = 0.01f;      // Delay between each bullet spawn

    void Start()
    {
        // Start firing the burst of bullets
        StartCoroutine(FireBurst());
    }

    private IEnumerator FireBurst()
    {
        for (int i = 0; i < burstBulletCount; i++)
        {
            Vector3 randomDirection = RandomDomeDirection();
            GameObject bullet = new GameObject("StrawberryBullet");
            bullet.transform.position = transform.position;

            Rigidbody rb = bullet.AddComponent<Rigidbody>();
            rb.useGravity = false;
            rb.velocity = randomDirection * bulletSpeed;

            SphereCollider col = bullet.AddComponent<SphereCollider>();
            col.isTrigger = true;

            Destroy(bullet, despawnTimer);
            yield return new WaitForSeconds(burstDelay);
        }
    }

    private Vector3 RandomDomeDirection()
    {
        // Generate a random direction vector in an upward dome shape
        float theta = Random.Range(0, Mathf.PI / 2); // Constrain to upper hemisphere
        float phi = Random.Range(0, Mathf.PI * 2);

        float x = Mathf.Sin(theta) * Mathf.Cos(phi);
        float y = Mathf.Cos(theta); // Positive y-axis for dome effect
        float z = Mathf.Sin(theta) * Mathf.Sin(phi);

        return new Vector3(x, y, z).normalized;
    }

    private void OnTriggerEnter(Collider other)
    {
        if (other.isTrigger || other.CompareTag("Enemy"))
        {
            return;
        }

        // Check if the object hit can take damage
        IDamage dmg = other.GetComponent<IDamage>();

        if (dmg != null)
        {
            // Apply damage to the object hit
            dmg.takeDamage(bulletDamage, -(transform.position - other.transform.position).normalized * (bulletDamage / 2), damageType.bullet);

            // Destroy the bullet on impact
            Destroy(gameObject);
        }
    }
}
