console.clear();
console.log("✅ Módulo Normas Iniciado de forma nativa.");

window.dTable = window.dTable || null;

// ===================================================================================================================== //
// FUNCIÓN AUXILIAR: REEMPLAZO DE ALERT() POR MODAL DINÁMICO
// ===================================================================================================================== //
function mostrarAlerta(mensaje, titulo = "Atención", esHtml = false) {
    let $alertaModal =$("#custom-alert-modal");

    if ($alertaModal.length === 0) {
        const modalHtml = `
        <div class="modal fade" id="custom-alert-modal" tabindex="-1" role="dialog" style="z-index: 12000;">
          <div class="modal-dialog modal-sm" role="document">
            <div class="modal-content" style="background-color: #1e1e24; color: #e0e0e0; border: 1px solid #3f424e; border-radius: 8px;">
              <div class="modal-header" style="border-bottom: 1px solid #3f424e;">
                <button type="button" class="close" data-dismiss="modal" aria-label="Close" style="color: #fff; opacity: 0.8; text-shadow: none;"><span aria-hidden="true">&times;</span></button>
                <h4 class="modal-title" style="color: #5dade2; font-weight: bold;">
                    <span class="glyphicon glyphicon-info-sign"></span> <span id="custom-alert-title"></span>
                </h4>
              </div>
              <div class="modal-body">
                <div id="custom-alert-message" style="font-size: 1.05em; margin-bottom: 0;"></div>
              </div>
              <div class="modal-footer" style="border-top: none; padding-top: 0;">
                <button type="button" class="btn btn-info btn-sm" data-dismiss="modal">Aceptar</button>
              </div>
            </div>
          </div>
        </div>`;
        $("body").append(modalHtml);
        $alertaModal =$("#custom-alert-modal");
    }

    $("#custom-alert-title").text(titulo);
    if (esHtml) {
        $("#custom-alert-message").html(mensaje);
    } else {
        $("#custom-alert-message").text(mensaje);
    }
    $alertaModal.modal("show");
}

(async () => {
  try {
    // ================================================================================================================= //
    // 1. CARGA DE LA TABLA MAESTRA DESDE EL BACKEND EN C
    // ================================================================================================================= //
    const response = await fetch(window.API_BASE_URL + "/normas/list");
    const registros = await response.json();

    const tableBody = document.getElementById("normasTableBody");
    const alertInfo = document.getElementById("alert-info-normas");
    tableBody.innerHTML = "";

    let count = 0;

    registros.forEach((reg) => {
      const row = document.createElement("tr");
      row.innerHTML = `
        <td class="text-center">${reg.nombre_norma}</td>
        <td class="text-center">${reg.n_norma}</td>
        <td class="text-center">${reg.tipo_norma}</td>
        <td class="text-center">${reg.f_norma}</td>
        <td class="text-center">${reg.f_pub}</td>
        <td class="text-center">${reg.anio_pub}</td>
        <td class="text-center">${reg.jurisdiccion}</td>
        <td class="text-center">${reg.organismo}</td>
        <td class="text-center">${reg.descripcion_organismo}</td>
        <td class="text-center">${reg.ubicacion_fisica}</td>
        <td class="text-center" style="white-space: nowrap;">
          <button class="btn btn-default btn-sm btn-editar-norma"
                  data-id="${reg.id}"
                  data-nombre_norma="${reg.nombre_norma}"
                  data-n_norma="${reg.n_norma}"
                  data-tipo_norma="${reg.tipo_norma}"
                  data-f_norma="${reg.f_norma}"
                  data-f_pub="${reg.f_pub}"
                  data-anio_pub="${reg.anio_pub}"
                  data-jurisdiccion="${reg.jurisdiccion}"
                  data-organismo="${reg.organismo}"
                  data-ubicacion_fisica="${reg.ubicacion_fisica}"
                  data-observaciones="${reg.observaciones}"
                  data-file_name="${reg.file_name}">
            <span class="glyphicon glyphicon-edit"></span> Editar
          </button>
          <button class="btn btn-info btn-sm btn-informe_extendido"
                  data-id="${reg.id}"
                  data-nombre_norma="${reg.nombre_norma}"
                  data-n_norma="${reg.n_norma}"
                  data-tipo_norma="${reg.tipo_norma}"
                  data-f_norma="${reg.f_norma}"
                  data-f_pub="${reg.f_pub}"
                  data-anio_pub="${reg.anio_pub}"
                  data-jurisdiccion="${reg.jurisdiccion}"
                  data-organismo="${reg.organismo}"
                  data-descripcion_organismo="${reg.descripcion_organismo}"
                  data-ubicacion_fisica="${reg.ubicacion_fisica}"
                  data-observaciones="${reg.observaciones}"
                  data-file_name="${reg.file_name}">
            <span class="glyphicon glyphicon-list-alt"></span> Información Extendida
          </button>
        </td>`;

      tableBody.appendChild(row);
      count++;
    });

    if (alertInfo) {
        alertInfo.innerHTML = `<div class="alert alert-info">
                                <span class="glyphicon glyphicon-option-vertical" aria-hidden="true"></span> <strong>Cantidad de Registros: </strong> ${count}
                               </div><hr>`;
    }

    if ($.fn.DataTable.isDataTable("#normasTable")) {
      $("#normasTable").DataTable().destroy();
    }

    dTable = $('#normasTable').DataTable({
        "order": [[0, "asc"]],
        "responsive":     true,
        "scrollY":        "300px",
        "scrollX":        true,
        "scrollCollapse": true,
        "paging":         true,
        "deferRender":    true,
        "retrieve":       true,
        "dom": '<"row"<"col-sm-12"l>><"row"<"col-sm-12"Bf>>rt<"row"<"col-sm-12"ip>>',
        buttons: [
            { extend: 'excel', text: '<i class="glyphicon glyphicon-list-alt"></i> Excel', exportOptions: { columns: ':visible'} },
            { extend: 'pdf', text: '<i class="glyphicon glyphicon-file"></i> PDF', exportOptions: { columns: ':visible'} },
            'colvis'
        ],
        "language": {
            "lengthMenu": "Mostrar _MENU_ registros por pagina",
            "info": "Mostrando pagina _PAGE_ de _PAGES_",
            "search": "Buscar:",
            "zeroRecords":    "No se encontraron registros coincidentes",
            "paginate": { "next": "Siguiente", "previous": "Anterior" }
        }
    });

    const contenedorModal = document.getElementById("contenedor-modal-maestro-normas");
    if (contenedorModal) {
        const respModal = await fetch(window.VIEWS_PATH + "/normas/nueva_norma.html");
        contenedorModal.innerHTML = await respModal.text();
    }

    $(document).off("click", "#add-norma-form").on("click", "#add-norma-form", function(e) {
        e.preventDefault();
        prepararYMostrarModal("alta");
    });

    $(document).off("click", ".btn-editar-norma").on("click", ".btn-editar-norma", function(e) {
        e.preventDefault();
        const datos = {
            id: $(this).attr("data-id"),
            nombre_norma: $(this).attr("data-nombre_norma"),
            n_norma: $(this).attr("data-n_norma"),
            tipo_norma: $(this).attr("data-tipo_norma"),
            f_norma: $(this).attr("data-f_norma"),
            f_pub: $(this).attr("data-f_pub"),
            anio_pub: $(this).attr("data-anio_pub"),
            jurisdiccion: $(this).attr("data-jurisdiccion"),
            organismo: $(this).attr("data-organismo"),
            ubicacion_fisica: $(this).attr("data-ubicacion_fisica"),
            observaciones: $(this).attr("data-observaciones"),
            file_name: $(this).attr("data-file_name")
        };
        prepararYMostrarModal("edicion", datos);
    });

    // Evento Información Extendida
    $(document).off("click", ".btn-informe_extendido").on("click", ".btn-informe_extendido", function(e) {
        e.preventDefault();
        const datos = {
            id: $(this).attr("data-id"),
            nombre_norma: $(this).attr("data-nombre_norma"),
            n_norma: $(this).attr("data-n_norma"),
            tipo_norma: $(this).attr("data-tipo_norma"),
            f_norma: $(this).attr("data-f_norma"),
            f_pub: $(this).attr("data-f_pub"),
            anio_pub: $(this).attr("data-anio_pub"),
            jurisdiccion: $(this).attr("data-jurisdiccion"),
            organismo: $(this).attr("data-organismo"),
            descripcion_organismo: $(this).attr("data-descripcion_organismo"),
            ubicacion_fisica: $(this).attr("data-ubicacion_fisica"),
            observaciones: $(this).attr("data-observaciones"),
            file_name: $(this).attr("data-file_name")
        };
        mostrarInformeExtendido(datos);
    });

  } catch (error) {
    console.error("💥 Error general en el módulo Normas:", error);
  }
})();

// ===================================================================================================================== //
// MODAL DE INFORMACIÓN EXTENDIDA CON VISOR LATERAL INTEGRADO
// ===================================================================================================================== //
async function mostrarInformeExtendido(datos) {
    let $modal =$("#modal-informe-extendido");

    // 🔹 NUEVO: Arreglo global para almacenar los nombres de los archivos vinculados para el reporte PDF
    let archivosParaReporte = [];

    if ($modal.length === 0) {
        const modalHtml = `
        <style>
            #modal-informe-extendido .modal-dialog {
                transition: width 0.3s ease;
                width: 750px;
                max-width: 95vw;
            }
            #modal-informe-extendido.expanded .modal-dialog {
                width: 1400px;
            }
            .ext-header-toolbar {
                display: flex;
                flex-wrap: wrap;
                gap: 8px;
                padding: 12px 15px;
                background-color: #24262c;
                border-bottom: 1px solid #3f424e;
            }
            .ext-header-toolbar .btn {
                background-color: #2b2d35;
                color: #e0e0e0;
                border: 1px solid #3f424e;
                font-weight: 600;
                font-size: 0.85em;
            }
            .ext-header-toolbar .btn:hover {
                background-color: #383b45;
                color: #ffffff;
            }
            .ext-info-row {
                display: flex;
                justify-content: space-between;
                align-items: center;
                padding: 9px 15px;
                border-bottom: 1px solid #333640;
                background-color: #2b2d35;
            }
            .ext-info-row:nth-child(even) { background-color: #26282e; }
            .ext-info-label { font-weight: 600; color: #a0a6b1; font-size: 0.88em; }
            .ext-info-badge {
                background-color: #3b5066;
                color: #e8f1f5;
                padding: 3px 10px;
                border-radius: 14px;
                font-weight: 600;
                font-size: 0.85em;
                text-align: right;
            }
            .ext-info-obs {
                padding: 12px 15px;
                background-color: #24262c;
                color: #cfd4dc;
                font-size: 0.9em;
                border-bottom: 1px solid #3f424e;
            }
            .ext-vinculadas-panel {
                padding: 12px 15px;
                background-color: #1e1e24;
            }
            .ext-vinculadas-title {
                font-weight: bold;
                color: #5dade2;
                margin-bottom: 8px;
                font-size: 0.9em;
                text-transform: uppercase;
                letter-spacing: 0.5px;
            }
            .ext-viewer-pane {
                background-color: #15161a;
                border-left: 1px solid #3f424e;
                height: 100%;
                min-height: 580px;
                display: none;
                flex-direction: column;
            }
            .ext-viewer-header {
                padding: 8px 12px;
                background-color: #24262c;
                border-bottom: 1px solid #3f424e;
                display: flex;
                justify-content: space-between;
                align-items: center;
                color: #5dade2;
                font-weight: bold;
                font-size: 0.9em;
            }
            .ext-viewer-body {
                flex: 1;
                position: relative;
                min-height: 540px;
            }
            .ext-viewer-body iframe {
                width: 100%;
                height: 100%;
                min-height: 540px;
                border: none;
            }
        </style>
        <div class="modal fade" id="modal-informe-extendido" tabindex="-1" role="dialog" style="z-index: 10500;">
          <div class="modal-dialog" role="document">
            <div class="modal-content" style="background-color: #1e1e24; border: 1px solid #3f424e; border-radius: 8px; overflow: hidden;">

              <div class="modal-header" style="border-bottom: 1px solid #3f424e; background-color: #2b2d35; padding: 10px 15px;">
                <button type="button" class="close" data-dismiss="modal" aria-label="Close" style="color: #fff; opacity: 0.8;"><span aria-hidden="true">&times;</span></button>
                <h4 class="modal-title" style="color: #5dade2; font-weight: bold; font-size: 1.1em;">
                    <span class="glyphicon glyphicon-tasks"></span> Información Extendida
                </h4>
              </div>

              <!-- TIRA DE BOTONES SUPERIOR -->
              <div class="ext-header-toolbar">
                <button class="btn btn-default btn-sm" data-dismiss="modal">
                    <span class="glyphicon glyphicon-arrow-left"></span> Volver a Normas
                </button>
                <button class="btn btn-default btn-sm" id="ext-btn-agregar-vinculada">
                    <span class="glyphicon glyphicon-paperclip text-success"></span> Agregar Normas Vinculadas
                </button>
                <button class="btn btn-default btn-sm" id="ext-btn-imprimir">
                    <span class="glyphicon glyphicon-print"></span> Imprimir Informe
                </button>
                <button class="btn btn-default btn-sm" id="ext-btn-ver-principal">
                    <span class="glyphicon glyphicon-file text-info"></span> Ver Norma Principal
                </button>
              </div>

              <div class="modal-body" style="padding: 0;">
                <div class="row" style="margin: 0; display: flex; flex-wrap: wrap;">
                  <!-- COLUMNA IZQUIERDA: DETALLES -->
                  <div class="col-xs-12" id="ext-details-col" style="padding: 0; transition: all 0.3s ease;">
                    <div id="ext-info-fields"></div>
                    <div class="ext-vinculadas-panel">
                        <div class="ext-vinculadas-title">Normas Vinculadas Asociadas</div>
                        <div id="ext-vinculadas-list-container"></div>
                    </div>
                  </div>

                  <!-- COLUMNA DERECHA: VISOR INCRUSTADO -->
                  <div class="col-xs-12" id="ext-viewer-col" style="padding: 0; display: none;">
                    <div class="ext-viewer-pane" id="ext-viewer-pane">
                      <div class="ext-viewer-header">
                        <span id="ext-viewer-title">Visualizador de Documento</span>
                        <button type="button" class="btn btn-xs btn-default" id="ext-btn-cerrar-visor" style="background:#2b2d35; color:#fff; border-color:#3f424e;">
                            <span class="glyphicon glyphicon-remove"></span> Ocultar Visor
                        </button>
                      </div>
                      <div class="ext-viewer-body">
                        <iframe id="ext-viewer-iframe" src=""></iframe>
                      </div>
                    </div>
                  </div>
                </div>
              </div>

            </div>
          </div>
        </div>`;
        $("body").append(modalHtml);
        $modal =$("#modal-informe-extendido");
    }

    $modal.removeClass("expanded");
    $("#ext-details-col").attr("class", "col-xs-12");
    $("#ext-viewer-col").hide();
    $("#ext-viewer-iframe").attr("src", "");

    const fieldsHtml = `
        <div class="ext-info-row">
            <span class="ext-info-label">Nombre de la Norma:</span>
            <span class="ext-info-badge">${datos.nombre_norma || '-'}</span>
        </div>
        <div class="ext-info-row">
            <span class="ext-info-label">Número de la Norma:</span>
            <span class="ext-info-badge">${datos.n_norma || '-'}</span>
        </div>
        <div class="ext-info-row">
            <span class="ext-info-label">Tipo de Norma:</span>
            <span class="ext-info-badge">${datos.tipo_norma || '-'}</span>
        </div>
        <div class="ext-info-row">
            <span class="ext-info-label">Ámbito de la Norma:</span>
            <span class="ext-info-badge">${datos.f_norma || '-'}</span>
        </div>
        <div class="ext-info-row">
            <span class="ext-info-label">Fecha Publicación:</span>
            <span class="ext-info-badge">${datos.f_pub || '-'}</span>
        </div>
        <div class="ext-info-row">
            <span class="ext-info-label">Año Publicación:</span>
            <span class="ext-info-badge">${datos.anio_pub || '-'}</span>
        </div>
        <div class="ext-info-row">
            <span class="ext-info-label">Organismo:</span>
            <span class="ext-info-badge">${datos.descripcion_organismo || datos.organismo || '-'}</span>
        </div>
        <div class="ext-info-row">
            <span class="ext-info-label">Jurisdicción:</span>
            <span class="ext-info-badge">${datos.jurisdiccion || '-'}</span>
        </div>
        <div class="ext-info-row">
            <span class="ext-info-label">Ubicación / Bibliorato:</span>
            <span class="ext-info-badge" style="background-color: #566573;">${datos.ubicacion_fisica || 'no tiene'}</span>
        </div>
        <div class="ext-info-obs">
            <strong style="color: #9aa0a6;">Observaciones:</strong> ${datos.observaciones || 'Sin observaciones.'}
        </div>
    `;
    $("#ext-info-fields").html(fieldsHtml);

    const incrustarDocumento = (blob, titulo) => {
        const urlVirtual = URL.createObjectURL(blob);
        $("#ext-viewer-title").text(titulo);
        $("#ext-viewer-iframe").attr("src", urlVirtual);

        $("#modal-informe-extendido").addClass("expanded");
        $("#ext-details-col").attr("class", "col-md-6 col-xs-12");
        $("#ext-viewer-col").attr("class", "col-md-6 col-xs-12").show();
        $("#ext-viewer-pane").css("display", "flex");
    };

    const refrescarVinculadas = async () => {
        const $cont =$("#ext-vinculadas-list-container");
        $cont.html(`<span class="text-info" style="font-size:0.88em;">⏳ Consultando normas vinculadas...</span>`);

        archivosParaReporte = []; // Resetear memoria de archivos

        try {
            const resp = await fetch(window.API_BASE_URL + "/normas_vinculadas/list");
            const lista = await resp.json();
            const vinculadas = lista.filter(item => item.id_norma_principal == datos.id);

            if (vinculadas.length === 0) {
                $cont.html(`
                    <div class="alert alert-warning" style="background-color: #2b271d; border-color: #614a19; color: #f39c12; margin-bottom: 0; padding: 10px; font-size: 0.88em;">
                        <span class="glyphicon glyphicon-info-sign"></span> <strong>Aviso:</strong> Aún no se han cargado normas vinculadas para esta norma principal.
                    </div>
                `);
                return;
            }

            let html = `<div class="list-group" style="margin-bottom: 0;">`;
            for (const v of vinculadas) {
                let archivos = [];
                try {
                    const rArch = await fetch(window.API_BASE_URL + "/normas_vinculadas/archivos", {
                        method: "POST",
                        headers: { "Content-Type": "application/x-www-form-urlencoded" },
                        body: `id=${v.id}`
                    });
                    if (rArch.ok) archivos = await rArch.json();
                } catch(e) { console.error(e); }

                if (archivos.length === 0) {
                    const fallbackName = v.path_name.split('/').pop();
                    archivos.push(fallbackName);
                }

                archivos.forEach(arch => {
                    archivosParaReporte.push(arch); // Guardamos para el PDF
                    html += `
                    <div class="list-group-item" style="background-color: #26282e; border-color: #3f424e; padding: 8px 12px; display: flex; justify-content: space-between; align-items: center;">
                        <span style="color: #e0e0e0; font-size: 0.88em; overflow: hidden; text-overflow: ellipsis; white-space: nowrap; max-width: 75%;">
                            <span class="glyphicon glyphicon-file text-info"></span> ${arch}
                        </span>
                        <button class="btn btn-xs btn-default btn-ver-archivo-vinculado" data-id="${v.id}" data-file="${arch}" style="background-color: #343741; color: #5dade2; border-color: #3f424e;">
                            <span class="glyphicon glyphicon-eye-open"></span> Visualizar
                        </button>
                    </div>`;
                });
            }
            html += `</div>`;
            $cont.html(html);

            $(".btn-ver-archivo-vinculado").off("click").on("click", async function() {
                const vid = $(this).attr("data-id");
                const arch = $(this).attr("data-file");

                try {
                    const r = await fetch(window.API_BASE_URL + "/normas_vinculadas/open_doc", {
                        method: "POST",
                        headers: { "Content-Type": "application/x-www-form-urlencoded" },
                        body: `id=${vid}&file_name=${encodeURIComponent(arch)}`
                    });
                    if (r.ok) {
                        const blob = await r.blob();
                        incrustarDocumento(blob, `Norma Vinculada: ${arch}`);
                    } else {
                        mostrarAlerta("No fue posible cargar el archivo físico.", "Error");
                    }
                } catch(err) {
                    console.error(err);
                    mostrarAlerta("Error de comunicación al abrir el documento.", "Error de Red");
                }
            });

        } catch (error) {
            console.error(error);
            $cont.html(`<span class="text-danger" style="font-size:0.88em;">Error al consultar normas vinculadas.</span>`);
        }
    };

    $("#ext-btn-ver-principal").off("click").on("click", async function() {
        if (!datos.file_name) {
            mostrarAlerta("Esta norma no posee un archivo principal asociado.", "Atención");
            return;
        }
        try {
            const resp = await fetch(window.API_BASE_URL + "/normas/open_doc", {
                method: "POST",
                headers: { "Content-Type": "application/x-www-form-urlencoded" },
                body: `id=${datos.id}`
            });
            if (resp.ok) {
                const blob = await resp.blob();
                incrustarDocumento(blob, `Norma Principal: ${datos.nombre_norma}`);
            } else {
                mostrarAlerta("No se pudo cargar el archivo de la norma principal.", "Error");
            }
        } catch(e) {
            console.error(e);
            mostrarAlerta("Fallo de red al solicitar el documento.", "Error");
        }
    });

    $("#ext-btn-cerrar-visor").off("click").on("click", function() {
        $("#modal-informe-extendido").removeClass("expanded");
        $("#ext-details-col").attr("class", "col-xs-12");
        $("#ext-viewer-col").hide();
        $("#ext-viewer-iframe").attr("src", "");
    });

    // ================================================================================= //
    // IMPRIMIR PDF: ESTILO CORPORATIVO CON ARCHIVOS VINCULADOS
    // ================================================================================= //
    $("#ext-btn-imprimir").off("click").on("click", function() {
        try {
            if (!window.jspdf) {
                mostrarAlerta("Librería jsPDF no detectada. Verifique que 'jspdf.umd.min.js' y el plugin autotable estén cargados en el index.", "Error Técnico");
                return;
            }

            const { jsPDF } = window.jspdf;
            const doc = new jsPDF();

            // Configuración de Colores Corporativos
            const primaryColor = [31, 58, 147]; // #1f3a93 (Azul Oscuro)
            const secondaryColor = [100, 100, 100];
            const headerFill = [43, 45, 53];

            // Cabecera Principal
            doc.setFontSize(22);
            doc.setTextColor(primaryColor[0], primaryColor[1], primaryColor[2]);
            doc.text("eGesDoc", 14, 20);

            doc.setFontSize(14);
            doc.setTextColor(40, 40, 40);
            doc.text("Reporte de Información Extendida", 14, 28);

            doc.setFontSize(10);
            doc.setTextColor(secondaryColor[0], secondaryColor[1], secondaryColor[2]);
            doc.text(`Fecha de emisión: ${new Date().toLocaleString()}`, 14, 34);

            // Línea separadora
            doc.setDrawColor(primaryColor[0], primaryColor[1], primaryColor[2]);
            doc.setLineWidth(0.5);
            doc.line(14, 38, 196, 38);

            // TABLA 1: Detalles de la Norma
            const tableData = [
                ["Nombre de la Norma:", datos.nombre_norma || "-"],
                ["Número de la Norma:", datos.n_norma || "-"],
                ["Tipo de Norma:", datos.tipo_norma || "-"],
                ["Ámbito de la Norma:", datos.f_norma || "-"],
                ["Fecha Publicación:", datos.f_pub || "-"],
                ["Año Publicación:", datos.anio_pub || "-"],
                ["Organismo:", datos.descripcion_organismo || datos.organismo || "-"],
                ["Jurisdicción:", datos.jurisdiccion || "-"],
                ["Ubicación / Bibliorato:", datos.ubicacion_fisica || "N/A"],
                ["Observaciones:", datos.observaciones || "Sin observaciones."]
            ];

            doc.autoTable({
                startY: 45,
                head: [["Atributo", "Detalle"]],
                body: tableData,
                theme: 'grid',
                headStyles: { fillColor: headerFill, fontSize: 11, fontStyle: 'bold' },
                styles: { fontSize: 10, cellPadding: 5, textColor: [40, 40, 40], lineColor: [200, 200, 200] },
                columnStyles: {
                    0: { fontStyle: 'bold', cellWidth: 55, fillColor: [245, 247, 250] },
                    1: { cellWidth: 'auto' }
                }
            });

            // TABLA 2: Listado de Documentos
            let currentY = doc.lastAutoTable.finalY + 15;

            doc.setFontSize(12);
            doc.setTextColor(primaryColor[0], primaryColor[1], primaryColor[2]);
            doc.text("Documentos Adjuntos y Vinculados", 14, currentY);

            currentY += 5;

            const archivosData = [];

            if (datos.file_name) {
                archivosData.push(["Norma Principal", datos.file_name]);
            } else {
                archivosData.push(["Norma Principal", "Sin archivo adjunto"]);
            }

            if (archivosParaReporte.length > 0) {
                archivosParaReporte.forEach(arch => {
                    archivosData.push(["Norma Vinculada", arch]);
                });
            } else {
                archivosData.push(["Normas Vinculadas", "No se encontraron archivos vinculados"]);
            }

            doc.autoTable({
                startY: currentY,
                head: [["Tipo de Documento", "Nombre del Archivo"]],
                body: archivosData,
                theme: 'plain',
                headStyles: { fillColor: [220, 225, 232], textColor: [40, 40, 40], fontStyle: 'bold' },
                styles: { fontSize: 10, cellPadding: 4, textColor: [60, 60, 60], borderBottomWidth: 0.1, lineColor: [200, 200, 200] },
                columnStyles: {
                    0: { fontStyle: 'italic', cellWidth: 55 },
                    1: { cellWidth: 'auto' }
                }
            });

            // Paginación (Footer)
            const pageCount = doc.internal.getNumberOfPages();
            for (let i = 1; i <= pageCount; i++) {
                doc.setPage(i);
                doc.setFontSize(8);
                doc.setTextColor(150, 150, 150);
                doc.text(`Generado por eGesDoc - Página ${i} de ${pageCount}`, 14, doc.internal.pageSize.height - 10);
            }

            // Descargar archivo
            const safeName = (datos.nombre_norma || "norma").replace(/[^a-zA-Z0-9]/g, "_");
            doc.save(`Informe_Corporativo_${safeName}_${datos.n_norma || datos.id}.pdf`);

        } catch (error) {
            console.error("Error al generar el PDF:", error);
            mostrarAlerta("Ocurrió un error al procesar el archivo PDF.", "Error");
        }
    });

    $("#ext-btn-agregar-vinculada").off("click").on("click", function() {
        abrirModalSubirVinculadas(datos, refrescarVinculadas);
    });

    $modal.modal("show");
    await refrescarVinculadas();
}

// ===================================================================================================================== //
// MODAL DEDICADO: SUBIDA MÚLTIPLE DE NORMAS VINCULADAS
// ===================================================================================================================== //
function abrirModalSubirVinculadas(datosNorma, onFinishCallback) {
    let $mSubir =$("#modal-subir-vinculadas");

    if ($mSubir.length === 0) {
        const modalHtml = `
        <div class="modal fade" id="modal-subir-vinculadas" tabindex="-1" role="dialog" style="z-index: 11500;">
          <div class="modal-dialog modal-sm" role="document" style="width: 480px; max-width: 95vw;">
            <div class="modal-content" style="background-color: #1e1e24; border: 1px solid #3f424e; border-radius: 8px; color: #e0e0e0;">

              <div class="modal-header" style="border-bottom: 1px solid #3f424e; background-color: #2b2d35;">
                <button type="button" class="close" data-dismiss="modal" style="color: #fff; opacity: 0.8;"><span aria-hidden="true">&times;</span></button>
                <h4 class="modal-title" style="color: #5dade2; font-weight: bold; font-size: 1.05em;">
                    <span class="glyphicon glyphicon-paperclip"></span> Añadir Normas Vinculadas
                </h4>
              </div>

              <div class="modal-body" style="padding: 20px;">
                <form id="form-subir-vinculadas">
                    <p style="font-size: 0.88em; color: #a0a6b1; margin-bottom: 15px;">
                        Adjuntar uno o más archivos vinculados a la norma: <br>
                        <strong style="color: #ffffff;" id="subir-vinculadas-norma-title"></strong>
                    </p>
                    <div class="form-group" style="background-color: #26282e; border: 1px dashed #5dade2; padding: 15px; border-radius: 6px;">
                        <label style="font-size: 0.85em; text-transform: uppercase; color: #9aa0a6;">Seleccionar Archivos (PDF, DOC, TXT):</label>
                        <input type="file" id="subir-vinculadas-files" class="form-control" multiple accept=".pdf,.doc,.docx,.txt" style="border:none; padding-left:0;" required>
                        <small class="text-muted" style="display:block; margin-top:5px;">Puede seleccionar varios archivos simultáneamente.</small>
                    </div>
                    <div id="subir-vinculadas-status" style="margin-top: 10px; font-weight: bold; font-size: 0.88em;"></div>
                </form>
              </div>

              <div class="modal-footer" style="border-top: 1px solid #3f424e; background-color: #24262c;">
                <button type="button" class="btn btn-default btn-sm" data-dismiss="modal">Cancelar</button>
                <button type="button" class="btn btn-success btn-sm" id="btn-ejecutar-subida-vinculadas">
                    <span class="glyphicon glyphicon-upload"></span> Subir Archivos
                </button>
              </div>

            </div>
          </div>
        </div>`;
        $("body").append(modalHtml);
        $mSubir =$("#modal-subir-vinculadas");
    }

    $("#subir-vinculadas-norma-title").text(`${datosNorma.nombre_norma} (${datosNorma.n_norma})`);
    $("#subir-vinculadas-files").val("");
    $("#subir-vinculadas-status").html("");

    $("#btn-ejecutar-subida-vinculadas").off("click").on("click", async function() {
        const fileInput = document.getElementById("subir-vinculadas-files");
        const statusBox = document.getElementById("subir-vinculadas-status");

        if (fileInput.files.length === 0) {
            statusBox.innerHTML = `<span class="text-warning">Por favor, seleccione al menos un archivo.</span>`;
            return;
        }

        const files = Array.from(fileInput.files);
        const safeNombre = (datosNorma.nombre_norma || "norma").replace(/[^a-zA-Z0-9_-]/g, "_");
        const safeNumero = (datosNorma.n_norma || datosNorma.id).replace(/[^a-zA-Z0-9_-]/g, "_");
        const carpetaNorma = `${safeNombre}_${safeNumero}`;

        statusBox.innerHTML = `<span class="text-info">⏳ Procesando y subiendo ${files.length} archivo(s)...</span>`;

        const leerArchivoBase64 = (file) => new Promise((resolve, reject) => {
            const reader = new FileReader();
            reader.onload = (e) => resolve(e.target.result);
            reader.onerror = (err) => reject(err);
            reader.readAsDataURL(file);
        });

        let subidosOk = 0;
        for (let i = 0; i < files.length; i++) {
            const f = files[i];
            try {
                const b64 = await leerArchivoBase64(f);
                const body = `id_norma_principal=${encodeURIComponent(datosNorma.id)}&carpeta_norma=${encodeURIComponent(carpetaNorma)}&file_name=${encodeURIComponent(f.name)}&file_data=${encodeURIComponent(b64)}`;

                const resp = await fetch(window.API_BASE_URL + "/normas_vinculadas/add", {
                    method: "POST",
                    headers: { "Content-Type": "application/x-www-form-urlencoded" },
                    body: body
                });

                if (resp.ok) {
                    const data = await resp.json();
                    if (data.status === "ok") subidosOk++;
                }
            } catch(e) {
                console.error("Error al subir archivo:", f.name, e);
            }
        }

        if (subidosOk === files.length) {
            statusBox.innerHTML = `<span class="text-success">✅ ¡${subidosOk} archivo(s) vinculados con éxito!</span>`;
            setTimeout(() => {
                $mSubir.modal("hide");
                if (typeof onFinishCallback === "function") onFinishCallback();
            }, 1000);
        } else {
            statusBox.innerHTML = `<span class="text-danger">⚠️ Se subieron ${subidosOk} de ${files.length} archivos. Revise la consola.</span>`;
            if (typeof onFinishCallback === "function") onFinishCallback();
        }
    });

    $mSubir.modal("show");
}

// ===================================================================================================================== //
// MODAL DE ALTA / EDICIÓN PRINCIPAL
// ===================================================================================================================== //
async function prepararYMostrarModal(modo, datos = null) {
    const $modal =$("#myModal-normas");
    if ($modal.length === 0) return;

    $modal.find(".modal-title").text(modo === "alta" ? "Añadir Norma" : "Editar Norma");

    $modal.find(".modal-body").html(`
        <style>
            .modern-dark-form {
                background-color: #1e1e24;
                padding: 20px 25px;
                border-radius: 8px;
                color: #e0e0e0;
                box-shadow: inset 0 0 10px rgba(0,0,0,0.5);
            }
            .modern-dark-form label {
                font-weight: 600;
                color: #9aa0a6;
                margin-bottom: 6px;
                font-size: 0.85em;
                text-transform: uppercase;
                letter-spacing: 0.5px;
            }
            .modern-dark-form .form-control {
                background-color: #2b2d35;
                border: 1px solid #3f424e;
                color: #ffffff;
                border-radius: 5px;
                transition: all 0.3s ease;
            }
            .modern-dark-form .form-control:focus {
                border-color: #5dade2;
                background-color: #343741;
                outline: 0;
                box-shadow: 0 0 0 0.15rem rgba(93, 173, 226, 0.25);
            }
            .modern-dark-form .form-control::placeholder { color: #6c757d; }
            .modern-dark-form .file-wrapper {
                background-color: #26282e;
                border: 1px dashed #5dade2;
                padding: 10px;
                border-radius: 5px;
                margin-top: 5px;
            }
        </style>

        <div class="modern-dark-form">
            <form id="form-modal-norma">
                <input type="hidden" id="modal-norma-id" value="${datos ? datos.id : ''}">

                <div class="row">
                    <div class="col-md-9">
                        <div class="form-group">
                            <label>Nombre Norma</label>
                            <input type="text" class="form-control" id="modal-norma-nombre_norma" value="${datos ? datos.nombre_norma : ''}" placeholder="Ej. Ley de Contrato de Trabajo" required>
                        </div>
                    </div>
                    <div class="col-md-3">
                        <div class="form-group">
                            <label>Nro. Norma</label>
                            <input type="text" class="form-control" id="modal-norma-n_norma" value="${datos ? datos.n_norma : ''}" required>
                        </div>
                    </div>
                </div>

                <div class="row">
                    <div class="col-md-6">
                        <div class="form-group">
                            <label>Tipo Norma</label>
                            <select class="form-control" id="modal-norma-tipo_norma" required></select>
                        </div>
                    </div>
                    <div class="col-md-6">
                        <div class="form-group">
                            <label>Ámbito Norma</label>
                            <select class="form-control" id="modal-norma-f_norma" required></select>
                        </div>
                    </div>
                </div>

                <div class="row">
                    <div class="col-md-6">
                        <div class="form-group">
                            <label>Fecha Publicación</label>
                            <input type="date" class="form-control" id="modal-norma-f_pub" value="${datos ? datos.f_pub : ''}" required>
                        </div>
                    </div>
                    <div class="col-md-6">
                        <div class="form-group">
                            <label>Año Publicación</label>
                            <input type="text" class="form-control" id="modal-norma-anio_pub" value="${datos ? datos.anio_pub : ''}" maxlength="4" placeholder="Ej. 1974" required>
                        </div>
                    </div>
                </div>

                <div class="row">
                    <div class="col-md-6">
                        <div class="form-group">
                            <label>Jurisdicción</label>
                            <select class="form-control" id="modal-norma-jurisdiccion" required></select>
                        </div>
                    </div>
                    <div class="col-md-6">
                        <div class="form-group">
                            <label>Organismo</label>
                            <select class="form-control" id="modal-norma-organismo" required></select>
                        </div>
                    </div>
                </div>

                <div class="row">
                    <div class="col-md-12">
                        <div class="form-group">
                            <label>Ubicación Física</label>
                            <input type="text" class="form-control" id="modal-norma-ubicacion_fisica" value="${datos ? datos.ubicacion_fisica : ''}" placeholder="Ej. Archivo Central, Estante 3" required>
                        </div>
                    </div>
                </div>

                <div class="row">
                    <div class="col-md-12">
                        <div class="form-group">
                            <label>Observaciones</label>
                            <textarea class="form-control" id="modal-norma-observaciones" rows="3" placeholder="Comentarios adicionales..." required>${datos ? datos.observaciones : ''}</textarea>
                        </div>
                    </div>
                </div>

                <div class="row">
                    <div class="col-md-12">
                        <div class="form-group file-wrapper">
                            <label>Documento Adjunto (PDF, DOC)</label>
                            <input type="file" class="form-control" style="border:none; padding-left:0;" id="modal-norma-file_name" accept=".pdf,.doc,.docx,.txt" ${datos ? '' : 'required'}>
                            ${datos && datos.file_name ? `<small class="text-info" style="display:block; margin-top:5px;">Archivo actual: <strong>${datos.file_name}</strong>. (Seleccione uno nuevo solo si desea reemplazarlo).</small>` : ''}
                        </div>
                    </div>
                </div>

                <button type="submit" id="btn-submit-oculto" style="display:none;"></button>
            </form>
        </div>
    `);

    $modal.find(".modal-footer").html(`
        <div id="modal-status-message" style="margin-bottom: 10px; text-align: left; font-weight: bold;"></div>
        <div class="text-right">
            <button type="button" class="btn btn-danger" data-dismiss="modal">
                <span class="glyphicon glyphicon-remove-circle" aria-hidden="true"></span> Cerrar</button>
            <button type="button" class="btn btn-success" id="btn-modal-guardar">
                <span class="glyphicon glyphicon-ok" aria-hidden="true"></span> Guardar</button>
        </div>
    `);

    let valTipoNorma = "";
    let valAmbito = "";
    let valJurisdiccion = "";
    let valOrganismo = "";

    if (datos !== null) {
        valTipoNorma = datos.tipo_norma;
        valAmbito = datos.f_norma;
        valJurisdiccion = datos.jurisdiccion;
        valOrganismo = datos.organismo;
    }

    await cargarComboTipoNorma(valTipoNorma);
    await cargarComboAmbitoNorma(valAmbito);
    await cargarComboJurisdicciones(valJurisdiccion);
    await cargarComboOrganismos(valOrganismo);

    $modal.modal("show");

    document.getElementById("btn-modal-guardar").onclick = () => {
        document.getElementById("btn-submit-oculto").click();
    };

    document.getElementById("form-modal-norma").onsubmit = async (e) => {
        e.preventDefault();

        const statusContainer = document.getElementById("modal-status-message");
        statusContainer.innerHTML = `<span class="text-info">⏳ Procesando solicitud y subiendo archivo...</span>`;

        const id = document.getElementById("modal-norma-id").value;
        const fileInput = document.getElementById("modal-norma-file_name");

        const payload = {
            nombre_norma: document.getElementById("modal-norma-nombre_norma").value,
            n_norma: document.getElementById("modal-norma-n_norma").value,
            tipo_norma: document.getElementById("modal-norma-tipo_norma").value,
            f_norma: document.getElementById("modal-norma-f_norma").value,
            f_pub: document.getElementById("modal-norma-f_pub").value,
            anio_pub: document.getElementById("modal-norma-anio_pub").value,
            jurisdiccion: document.getElementById("modal-norma-jurisdiccion").value,
            organismo: document.getElementById("modal-norma-organismo").value,
            ubicacion_fisica: document.getElementById("modal-norma-ubicacion_fisica").value,
            observaciones: document.getElementById("modal-norma-observaciones").value
        };

        const executeFetch = async (finalFileName, finalFileData) => {
            try {
                const endpoint = id == "" ? "/normas/add" : "/normas/edit";
                let bodyData = `id=${encodeURIComponent(id)}&nombre_norma=${encodeURIComponent(payload.nombre_norma)}&n_norma=${encodeURIComponent(payload.n_norma)}&tipo_norma=${encodeURIComponent(payload.tipo_norma)}&f_norma=${encodeURIComponent(payload.f_norma)}&f_pub=${encodeURIComponent(payload.f_pub)}&anio_pub=${encodeURIComponent(payload.anio_pub)}&jurisdiccion=${encodeURIComponent(payload.jurisdiccion)}&organismo=${encodeURIComponent(payload.organismo)}&ubicacion_fisica=${encodeURIComponent(payload.ubicacion_fisica)}&observaciones=${encodeURIComponent(payload.observaciones)}&file_name=${encodeURIComponent(finalFileName)}`;

                if (finalFileData !== "") {
                    bodyData += `&file_data=${encodeURIComponent(finalFileData)}`;
                }

                const enviorresp = await fetch(window.API_BASE_URL + endpoint, {
                    method: "POST",
                    headers: { "Content-Type": "application/x-www-form-urlencoded" },
                    body: bodyData
                });

                const data = await enviorresp.json();

                if (enviorresp.ok && data.status === "ok") {
                    statusContainer.innerHTML = `<span class="text-success"><strong>✅ ¡Éxito!</strong> ${data.message}.</span>`;
                    setTimeout(() => {
                        $modal.modal("hide");
                        window.loadDashboardView(window.VIEWS_PATH + "/normas/normas.html");
                    }, 1200);
                } else {
                    statusContainer.innerHTML = `<span class="text-danger"><strong>❌ Error:</strong> ${data.message}.</span>`;
                }
            } catch (err) {
                console.error(err);
                statusContainer.innerHTML = `<span class="text-danger"><strong>💥 Error:</strong> Fallo al conectar con el servidor.</span>`;
            }
        };

        if (fileInput.files.length > 0) {
            const file = fileInput.files[0];
            const reader = new FileReader();
            reader.onload = async function(event) {
                const fileBase64 = event.target.result;
                await executeFetch(file.name, fileBase64);
            };
            reader.readAsDataURL(file);
        } else {
            const existingFileName = datos ? datos.file_name : "";
            await executeFetch(existingFileName, "");
        }
    };
}

async function cargarComboJurisdicciones(valorSeleccionado) {
    const select = document.getElementById("modal-norma-jurisdiccion");
    if (!select) return;
    try {
        const resp = await fetch(window.API_BASE_URL + "/jurisdicciones/list");
        const lista = await resp.json();
        select.innerHTML = `<option value="">-- Seleccionar --</option>`;
        lista.forEach(item => {
            const cod = item.cod_jur ? item.cod_jur.trim() : "";
            const desc = item.descripcion ? item.descripcion.trim() : "";
            const opt = document.createElement("option");
            opt.value = desc;
            opt.textContent = `${cod} - ${desc}`;
            if (desc == valorSeleccionado) opt.selected = true;
            select.appendChild(opt);
        });
    } catch (err) {
        console.error("Error al cargar Jurisdicciones:", err);
    }
}

async function cargarComboOrganismos(valorSeleccionado) {
    const select = document.getElementById("modal-norma-organismo");
    if (!select) return;
    try {
        const resp = await fetch(window.API_BASE_URL + "/organismos/list");
        const lista = await resp.json();
        select.innerHTML = `<option value="">-- Seleccionar --</option>`;
        lista.forEach(item => {
            const cod = item.cod_org ? item.cod_org.trim() : "";
            const desc = item.descripcion ? item.descripcion.trim() : "";
            const opt = document.createElement("option");
            opt.value = cod;
            opt.textContent = `${cod} - ${desc}`;
            if (cod == valorSeleccionado) opt.selected = true;
            select.appendChild(opt);
        });
    } catch (err) {
        console.error("Error al cargar Organismos:", err);
    }
}

async function cargarComboTipoNorma(valorSeleccionado) {
    const select = document.getElementById("modal-norma-tipo_norma");
    if (!select) return;
    try {
        const resp = await fetch(window.API_BASE_URL + "/tipo_norma/list");
        const lista = await resp.json();
        select.innerHTML = `<option value="">-- Seleccionar --</option>`;
        lista.forEach(item => {
            const desc = item.descripcion ? item.descripcion.trim() : "";
            const opt = document.createElement("option");
            opt.value = desc;
            opt.textContent = `${desc}`;
            if (desc == valorSeleccionado) opt.selected = true;
            select.appendChild(opt);
        });
    } catch (err) {
        console.error("Error al cargar Tipo Norma:", err);
    }
}

async function cargarComboAmbitoNorma(valorSeleccionado) {
    const select = document.getElementById("modal-norma-f_norma");
    if (!select) return;
    try {
        const resp = await fetch(window.API_BASE_URL + "/ambito_norma/list");
        const lista = await resp.json();
        select.innerHTML = `<option value="">-- Seleccionar --</option>`;
        lista.forEach(item => {
            const desc = item.descripcion ? item.descripcion.trim() : "";
            const opt = document.createElement("option");
            opt.value = desc;
            opt.textContent = `${desc}`;
            if (desc == valorSeleccionado) opt.selected = true;
            select.appendChild(opt);
        });
    } catch (err) {
        console.error("Error al cargar Ambito Norma:", err);
    }
}
