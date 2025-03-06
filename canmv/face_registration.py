from libs.PipeLine import PipeLine, ScopedTiming
from libs.AIBase import AIBase
from libs.AI2D import Ai2d
from media.media import *
from time import *  #检查是否可去除
import os, gc, sys, math, random, time
import ujson
import nncase_runtime as nn
import ulab.numpy as np
import image
import aidemo

# 自定义人脸检测任务类
class FaceDetApp(AIBase):   #继承自父类AIBase
    def __init__(self,kmodel_path,model_input_size,anchors,confidence_threshold=0.25,nms_threshold=0.3,rgb888p_size=[1280,720],display_size=[1920,1080],debug_mode=0):
        super().__init__(kmodel_path,model_input_size,rgb888p_size,debug_mode)  #调用父类构造函数
        self.kmodel_path=kmodel_path
        self.model_input_size=model_input_size
        self.confidence_threshold=confidence_threshold  # 置信度阈值
        # nms较大时保留更多的重叠框，可能会导致同一个目标被多次检测到。nms较小时，严格限制重叠框，可能会导致一些目标被漏检
        self.nms_threshold=nms_threshold    # nms阈值用于控制框的重叠度。
        self.anchors=anchors    # ？？？？？？？？？？？？？？？？？
        # ALIGN_UP为对齐函数，将数值向上对齐到指定的倍数
        self.rgb888p_size=[ALIGN_UP(rgb888p_size[0],16),rgb888p_size[1]]    # sensor给到AI的图像分辨率，宽16字节对齐
        self.display_size=[ALIGN_UP(display_size[0],16),display_size[1]]    # 视频输出分辨率，宽16字节对齐
        self.debug_mode=debug_mode
        self.ai2d=Ai2d(debug_mode)  # 实例化Ai2d，用于实现模型预处理
        self.ai2d.set_ai2d_dtype(nn.ai2d_format.NCHW_FMT,nn.ai2d_format.NCHW_FMT,np.uint8, np.uint8)    # 设置Ai2d的输入输出格式和类型
        self.image_size=[]

    # 配置预处理
    # Ai2d支持crop/shift/pad/resize/affine，代码位于/sdcard/libs/AI2D.py
    def config_preprocess(self,input_image_size=None):
        # ScopedTiming 类在PipeLine.py模块内，是一个用来测量代码块执行时间的上下文管理器
        with ScopedTiming("set preprocess config",self.debug_mode > 0):
            # 初始化ai2d预处理配置
            ai2d_input_size=input_image_size if input_image_size else self.rgb888p_size
            self.image_size=[input_image_size[1],input_image_size[0]]
            self.ai2d.pad(self.get_pad_param(ai2d_input_size), 0, [104,117,123])    # 设置padding预处理（填充）
            self.ai2d.resize(nn.interp_method.tf_bilinear, nn.interp_mode.half_pixel)   # 设置resize预处理（缩放）
            # 构建预处理流程,参数为预处理输入tensor的shape和预处理输出的tensor的shape
            self.ai2d.build([1,3,ai2d_input_size[1],ai2d_input_size[0]],[1,3,self.model_input_size[1],self.model_input_size[0]])

    # 后处理
    def postprocess(self,results):
        with ScopedTiming("postprocess",self.debug_mode > 0):
            # aidemo库的face_det_post_process接口
            res = aidemo.face_det_post_process(self.confidence_threshold,self.nms_threshold,self.model_input_size[0],self.anchors,self.image_size,results)
            if len(res)==0:
                return res
            else:
                return res[0],res[1]

    # 获取缩放参数
    def get_pad_param(self,image_input_size):
        dst_w = self.model_input_size[0]
        dst_h = self.model_input_size[1]
        # 计算最小的缩放比例，等比例缩放
        ratio_w = dst_w / image_input_size[0]
        ratio_h = dst_h / image_input_size[1]
        if ratio_w < ratio_h:
            ratio = ratio_w
        else:
            ratio = ratio_h
        new_w = (int)(ratio * image_input_size[0])
        new_h = (int)(ratio * image_input_size[1])
        # 计算填充参数
        dw = (dst_w - new_w) / 2
        dh = (dst_h - new_h) / 2
        top = (int)(round(0))   # round取整函数
        bottom = (int)(round(dh * 2 + 0.1))
        left = (int)(round(0))
        right = (int)(round(dw * 2 - 0.1))
        return [0,0,0,0,top, bottom, left, right]

# 自定义人脸注册任务类
class FaceRegistrationApp(AIBase):
    def __init__(self,kmodel_path,model_input_size,rgb888p_size=[1920,1080],display_size=[1920,1080],debug_mode=0):
        super().__init__(kmodel_path,model_input_size,rgb888p_size,debug_mode)
        self.kmodel_path=kmodel_path
        self.model_input_size=model_input_size
        self.rgb888p_size=[ALIGN_UP(rgb888p_size[0],16),rgb888p_size[1]]
        self.display_size=[ALIGN_UP(display_size[0],16),display_size[1]]
        self.debug_mode=debug_mode
        # 标准5官
        self.umeyama_args_112 = [
            38.2946 , 51.6963 ,
            73.5318 , 51.5014 ,
            56.0252 , 71.7366 ,
            41.5493 , 92.3655 ,
            70.7299 , 92.2041
        ]
        self.ai2d=Ai2d(debug_mode)
        self.ai2d.set_ai2d_dtype(nn.ai2d_format.NCHW_FMT,nn.ai2d_format.NCHW_FMT,np.uint8, np.uint8)

    # 配置预处理
    def config_preprocess(self,landm,input_image_size=None):
        with ScopedTiming("set preprocess config",self.debug_mode > 0):
            ai2d_input_size=input_image_size if input_image_size else self.rgb888p_size
            # 计算affine矩阵，并设置仿射变换预处理
            affine_matrix = self.get_affine_matrix(landm)
            self.ai2d.affine(nn.interp_method.cv2_bilinear,0, 0, 127, 1,affine_matrix)
            # 构建预处理流程,参数为预处理输入tensor的shape和预处理输出的tensor的shape
            self.ai2d.build([1,3,ai2d_input_size[1],ai2d_input_size[0]],[1,3,self.model_input_size[1],self.model_input_size[0]])

    # 后处理
    def postprocess(self,results):
        with ScopedTiming("postprocess",self.debug_mode > 0):
            return results[0][0]

    # 2x2矩阵的奇异值分解
    # SVD是一种矩阵分解方法，将一个矩阵分解为三个特定类型的矩阵的乘积：正交矩阵（U）、对角矩阵（S），以及正交矩阵（V）
    def svd22(self,a):
        # svd
        s = [0.0, 0.0]
        u = [0.0, 0.0, 0.0, 0.0]
        v = [0.0, 0.0, 0.0, 0.0]
        s[0] = (math.sqrt((a[0] - a[3]) ** 2 + (a[1] + a[2]) ** 2) + math.sqrt((a[0] + a[3]) ** 2 + (a[1] - a[2]) ** 2)) / 2
        s[1] = abs(s[0] - math.sqrt((a[0] - a[3]) ** 2 + (a[1] + a[2]) ** 2))
        v[2] = math.sin((math.atan2(2 * (a[0] * a[1] + a[2] * a[3]), a[0] ** 2 - a[1] ** 2 + a[2] ** 2 - a[3] ** 2)) / 2) if \
        s[0] > s[1] else 0
        v[0] = math.sqrt(1 - v[2] ** 2)
        v[1] = -v[2]
        v[3] = v[0]
        u[0] = -(a[0] * v[0] + a[1] * v[2]) / s[0] if s[0] != 0 else 1
        u[2] = -(a[2] * v[0] + a[3] * v[2]) / s[0] if s[0] != 0 else 0
        u[1] = (a[0] * v[1] + a[1] * v[3]) / s[1] if s[1] != 0 else -u[2]
        u[3] = (a[2] * v[1] + a[3] * v[3]) / s[1] if s[1] != 0 else u[0]
        v[0] = -v[0]
        v[2] = -v[2]
        return u, s, v

    # 使用Umeyama算法计算仿射变换矩阵
    def image_umeyama_112(self,src):
        SRC_NUM = 5
        SRC_DIM = 2
        src_mean = [0.0, 0.0]
        dst_mean = [0.0, 0.0]
        for i in range(0,SRC_NUM * 2,2):
            src_mean[0] += src[i]
            src_mean[1] += src[i + 1]
            dst_mean[0] += self.umeyama_args_112[i]
            dst_mean[1] += self.umeyama_args_112[i + 1]
        src_mean[0] /= SRC_NUM
        src_mean[1] /= SRC_NUM
        dst_mean[0] /= SRC_NUM
        dst_mean[1] /= SRC_NUM
        src_demean = [[0.0, 0.0] for _ in range(SRC_NUM)]
        dst_demean = [[0.0, 0.0] for _ in range(SRC_NUM)]
        for i in range(SRC_NUM):
            src_demean[i][0] = src[2 * i] - src_mean[0]
            src_demean[i][1] = src[2 * i + 1] - src_mean[1]
            dst_demean[i][0] = self.umeyama_args_112[2 * i] - dst_mean[0]
            dst_demean[i][1] = self.umeyama_args_112[2 * i + 1] - dst_mean[1]
        A = [[0.0, 0.0], [0.0, 0.0]]
        for i in range(SRC_DIM):
            for k in range(SRC_DIM):
                for j in range(SRC_NUM):
                    A[i][k] += dst_demean[j][i] * src_demean[j][k]
                A[i][k] /= SRC_NUM
        T = [[1, 0, 0], [0, 1, 0], [0, 0, 1]]
        U, S, V = self.svd22([A[0][0], A[0][1], A[1][0], A[1][1]])
        T[0][0] = U[0] * V[0] + U[1] * V[2]
        T[0][1] = U[0] * V[1] + U[1] * V[3]
        T[1][0] = U[2] * V[0] + U[3] * V[2]
        T[1][1] = U[2] * V[1] + U[3] * V[3]
        scale = 1.0
        src_demean_mean = [0.0, 0.0]
        src_demean_var = [0.0, 0.0]
        for i in range(SRC_NUM):
            src_demean_mean[0] += src_demean[i][0]
            src_demean_mean[1] += src_demean[i][1]
        src_demean_mean[0] /= SRC_NUM
        src_demean_mean[1] /= SRC_NUM
        for i in range(SRC_NUM):
            src_demean_var[0] += (src_demean_mean[0] - src_demean[i][0]) * (src_demean_mean[0] - src_demean[i][0])
            src_demean_var[1] += (src_demean_mean[1] - src_demean[i][1]) * (src_demean_mean[1] - src_demean[i][1])
        src_demean_var[0] /= SRC_NUM
        src_demean_var[1] /= SRC_NUM
        scale = 1.0 / (src_demean_var[0] + src_demean_var[1]) * (S[0] + S[1])
        T[0][2] = dst_mean[0] - scale * (T[0][0] * src_mean[0] + T[0][1] * src_mean[1])
        T[1][2] = dst_mean[1] - scale * (T[1][0] * src_mean[0] + T[1][1] * src_mean[1])
        T[0][0] *= scale
        T[0][1] *= scale
        T[1][0] *= scale
        T[1][1] *= scale
        return T

    # 获取affine变换矩阵
    def get_affine_matrix(self,sparse_points):
        with ScopedTiming("get_affine_matrix", self.debug_mode > 1):
            # 使用Umeyama算法计算仿射变换矩阵
            matrix_dst = self.image_umeyama_112(sparse_points)
            matrix_dst = [matrix_dst[0][0],matrix_dst[0][1],matrix_dst[0][2],
                          matrix_dst[1][0],matrix_dst[1][1],matrix_dst[1][2]]
            return matrix_dst

# 人脸注册任务类
class FaceRegistration:
    def __init__(self,face_det_kmodel,face_reg_kmodel,det_input_size,reg_input_size,database_dir,anchors,confidence_threshold=0.25,nms_threshold=0.3,rgb888p_size=[1280,720],display_size=[1920,1080],debug_mode=0):
        self.face_det_kmodel=face_det_kmodel    # 人脸检测模型路径
        self.face_reg_kmodel=face_reg_kmodel    # 人脸注册模型路径
        self.det_input_size=det_input_size      # 人脸检测模型输入分辨率
        self.reg_input_size=reg_input_size      # 人脸注册模型输入分辨率
        self.database_dir=database_dir
        self.anchors=anchors
        self.confidence_threshold=confidence_threshold
        self.nms_threshold=nms_threshold
        self.rgb888p_size=[ALIGN_UP(rgb888p_size[0],16),rgb888p_size[1]]
        self.display_size=[ALIGN_UP(display_size[0],16),display_size[1]]
        self.debug_mode=debug_mode
        self.face_det=FaceDetApp(self.face_det_kmodel,model_input_size=self.det_input_size,anchors=self.anchors,confidence_threshold=self.confidence_threshold,nms_threshold=self.nms_threshold,debug_mode=0)
        self.face_reg=FaceRegistrationApp(self.face_reg_kmodel,model_input_size=self.reg_input_size,rgb888p_size=self.rgb888p_size)

    def run(self,input_np,img_file):
        self.face_det.config_preprocess(input_image_size=[input_np.shape[3],input_np.shape[2]])
        # 该run函数来自FaceDetApp父类AIBase。执行模型推理全部过程，包含预处理、推理、获取输出、后处理，返回后处理输出，并在Display绘制结果
#        det_boxes,landms = self.face_det.run(input_np)
        try:
            det_boxes,landms = self.face_det.run(input_np)
        except ValueError:
            det_boxes, landms = None, None

        if det_boxes:
            if det_boxes.shape[0] == 1:
                # 只检测到一张人脸时，则将该人脸注册到数据库
                db_i_name = img_file.split('.')[0]
                for landm in landms:
                    self.face_reg.config_preprocess(landm,input_image_size=[input_np.shape[3],input_np.shape[2]])
                    reg_result = self.face_reg.run(input_np)
                    with open(self.database_dir+'{}.bin'.format(db_i_name), "wb") as file:
                        file.write(reg_result.tobytes())
                        print('Face registration success!')
                        return 1
            else:
                print('Only one person in a picture when you sign up')
                return -1
        else:
            print('No person detected')
            return -1

    # 将Image转换为rgb888格式
    def image2rgb888array(self,img):   #4维
        with ScopedTiming("fr_kpu_deinit",self.debug_mode > 0):
            img_data_rgb888=img.to_rgb888()
            # hwc,rgb888
            img_hwc=img_data_rgb888.to_numpy_ref()
            shape=img_hwc.shape
            img_tmp = img_hwc.reshape((shape[0] * shape[1], shape[2]))
            img_tmp_trans = img_tmp.transpose()
            img_res=img_tmp_trans.copy()
            # chw,rgb888
            img_return=img_res.reshape((1,shape[2],shape[0],shape[1]))
        return  img_return

    def dimension_convert(self,img):   #4维
        img_res = img.copy()
        img_return = img_res.reshape((1,img.shape[0],img.shape[1],img.shape[2]))
        return  img_return

def face_reg_init():
    face_det_kmodel_path="/data/face_model/face_detection_320.kmodel"
    face_reg_kmodel_path="/data/face_model/face_recognition.kmodel"
    anchors_path="/sdcard/examples/utils/prior_data_320.bin"
    database_dir="/data/face_reg/"
    database_img_dir="/data/face_reg/"
    face_det_input_size=[320,320]
    face_reg_input_size=[112,112]
    confidence_threshold=0.5
    nms_threshold=0.2
    anchor_len=4200
    det_dim=4
    anchors = np.fromfile(anchors_path, dtype=np.float)
    anchors = anchors.reshape((anchor_len,det_dim))
    #    max_register_face = 100              #数据库最多人脸个数
    #    feature_num = 128                    #人脸识别特征维度

    return FaceRegistration(face_det_kmodel_path,face_reg_kmodel_path,det_input_size=face_det_input_size,reg_input_size=face_reg_input_size,database_dir=database_dir,anchors=anchors,confidence_threshold=confidence_threshold,nms_threshold=nms_threshold)

if __name__=="__main__":
    freg = face_reg_init()
    # 获取图像列表
    database_img_dir="/data/face_reg/"
    img_list = os.listdir(database_img_dir)
    for img_file in img_list:
        #本地读取一张图像
        if img_file.endswith(".jpg"):
            full_img_file = database_img_dir + img_file
            print(full_img_file)
            img = image.Image(full_img_file)
            img.compress_for_ide()  # 图片压缩，参数为压缩质量，0-100，默认质量为50
            # 转rgb888的chw格式
            rgb888p_img_ndarry = freg.image2rgb888array(img)
            print(rgb888p_img_ndarry.shape)
            # 人脸注册
            freg.run(rgb888p_img_ndarry,img_file)
            gc.collect()


