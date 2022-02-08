using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BreakBox : MonoBehaviour
{
    [SerializeField]
    private float impulse = 4.0f;

    [SerializeField]
    private GameObject destroyBoxPrefab;

    // Start is called before the first frame update
    void Start()
    {

    }

    // Update is called once per frame
    void Update()
    {

    }
    //���̉����ɓ����������̏���
    private void OnTriggerEnter(Collider other)
    {
        //�v���C���[�ɏՓ˂����ꍇ
        Player player = other.GetComponent<Player>();
        if (player != null)
        {
            //�j�󔠐���
            GameObject destroyBox = Instantiate(destroyBoxPrefab, transform.position, transform.rotation);

            //�j�󔠂̎q�̑S�Ă̔j�Ђ�񋓂���
            foreach(Rigidbody body in destroyBox.GetComponentsInChildren<Rigidbody>())
            {
                //�j�ЂɃ����_���ɏՌ���^����
                body.AddForce(
                    Random.Range(-impulse, impulse),
                    Random.Range(0.0f, impulse),
                    Random.Range(-impulse, impulse),
                    ForceMode.Impulse
                    );
            }

            //�w�莞�Ԍ�ɔj�󔠂��폜
            Destroy(destroyBox, 3.0f);

            //�������폜
            Destroy(gameObject);
        }
    }
}
