DELIMITER //


CREATE PROCEDURE sp_insertar_normas(
    IN p_nombre_norma VARCHAR(500),
    IN p_n_norma VARCHAR(100),
    IN p_tipo_norma VARCHAR(200),
    IN p_f_norma VARCHAR(200),
    IN p_f_pub DATE,
    IN p_anio_pub VARCHAR(4),
    IN p_jurisdiccion VARCHAR(100),
    IN p_organismo VARCHAR(100),
    IN p_unidad_fisica VARCHAR(100),
    IN p_observaciones VARCHAR(3000),
    IN p_file_name VARCHAR(100),
    IN p_file_path VARCHAR(100)
)
BEGIN
    INSERT INTO normas (nombre_norma, n_norma, tipo_norma, f_norma, f_pub, anio_pub, jurisdiccion, organismo, unidad_fisica, observaciones, file_name, file_path)
    VALUES (p_nombre_norma, p_n_norma, p_tipo_norma, p_f_norma, p_f_pub, p_anio_pub, p_jurisdiccion, p_organismo, p_unidad_fisica, p_observaciones, p_file_name, p_file_path);
    SELECT LAST_INSERT_ID() AS nuevo_id;
END //

CREATE PROCEDURE sp_editar_normas(
    IN p_id INT,
    IN p_nombre_norma VARCHAR(500),
    IN p_n_norma VARCHAR(100),
    IN p_tipo_norma VARCHAR(200),
    IN p_f_norma VARCHAR(200),
    IN p_f_pub DATE,
    IN p_anio_pub VARCHAR(4),
    IN p_jurisdiccion VARCHAR(100),
    IN p_organismo VARCHAR(100),
    IN p_unidad_fisica VARCHAR(100),
    IN p_observaciones VARCHAR(3000),
    IN p_file_name VARCHAR(100),
    IN p_file_path VARCHAR(100)
)
BEGIN
    UPDATE normas
    SET nombre_norma = p_nombre_norma, n_norma = p_n_norma, tipo_norma = p_tipo_norma, f_norma = p_f_norma, f_pub = p_f_pub, anio_pub = p_anio_pub, jurisdiccion = p_jurisdiccion, organismo = p_organismo, unidad_fisica = p_unidad_fisica, observaciones = p_observaciones, file_name = p_file_name, file_path = p_file_path
    WHERE id = p_id;
END //

DELIMITER ;
