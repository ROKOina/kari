using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Coin : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        //Y����]�i�P�b�ԂɂP�W�O�x��]����j
        transform.Rotate(0.0f, 180 * Time.deltaTime, 0.0f);
    }

    //�Փ˂������ɌĂ΂��֐�
    private void OnTriggerEnter(Collider other)
    {
        //�v���C���[�ƏՓ˂�����
        Player player = other.GetComponent<Player>();
        if(player!=null)
        {
            //�R�C���l��
            player.AddCoin(1);

            //���M��j������
            Destroy(gameObject);
        }
        
    }
}
