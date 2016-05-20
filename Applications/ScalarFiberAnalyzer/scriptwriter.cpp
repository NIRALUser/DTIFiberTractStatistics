#include "scriptwriter.h"

ScriptWriter* ScriptWriter::writer = NULL;

ScriptWriter* ScriptWriter::getInstance(QString tool, QString pipeline){
    if(!writer)
        writer = new ScriptWriter(tool, pipeline);
    return writer;
}

// to-do: enforce pathname to end with .py
ScriptWriter::ScriptWriter(QString tool,QString pipeline){
    tool_script = tool;
    pipeline_script = pipeline;
    hasTool = false;
}


void ScriptWriter::writePreliminary(){

    QFile file(tool_script);

    if(file.open(QIODevice::WriteOnly)){
        file.write("#! /usr/bin/env python\n");
        file.write("# -*- coding: utf-8 -*-\n\n");
        file.write("import sys\n");
        file.write("if sys.version_info<(2,5):\n");
        file.write("\tprint(\"false\")\n");
        file.write("else:\n");
        file.write("\tprint(\"true\")\n");
        file.close();
    }else{
        // to-do: open failed. Should throw an error
        ErrorReporter::fire("Failed to write into file " + tool_script.toStdString() + " for script generation. Maybe you don't have write access to the executing path?");
    }


}

// issue: special characters are not escaped
bool ScriptWriter::writeData(QString outdir, QString fiber_dir, QString fiber_process, QString dtistat, bool isHField, QString scalar_name, std::vector<tool::MapData> data,
                             std::vector<tool::TractData> tracts, bool isTranspose)
{
    // issue: path may not take slash in Window system
    QFile file(outdir + "/" +pipeline_script);

    // override without warning if file exists
    if(file.open(QIODevice::WriteOnly))
    {
        file.write("#! /usr/bin/env python\n");
        file.write("# -*- coding: utf-8 -*-\n\n");
        file.write("import sys\n");
        file.write("import subprocess\n");
        file.write("import string\n");
        file.write("import os\n");
        file.write("import csv\n");
        file.write("import time\n\n");

        // global declaration
        file.write("# global declaration \n");
        char out[64+outdir.size()];
        snprintf(out,sizeof out, "out_dir = \"%s\"\n",outdir.toStdString().c_str());
        file.write(out);
        char fout[64+ outdir.size()];
        snprintf(fout,sizeof fout, "fiber_dir = \"%s\"\n",fiber_dir.toStdString().c_str());
        char fiberprocess[64+ fiber_process.size()];
        snprintf(fiberprocess,sizeof fiberprocess, "fiberprocess_path = \"%s\"\n",fiber_process.toStdString().c_str());
        char dtistatout[64 + dtistat.size()];
        snprintf(dtistatout,sizeof dtistatout, "dti_stat_path= \"%s\"\n", dtistat.toStdString().c_str());

        file.write(fout);
        file.write(fiberprocess);
        file.write(dtistatout);
        file.write("fvp_results=[]\n\n");

        file.write("# procedure run for each case\n");

        file.write("def run_process(sid,scalar_path,def_path,fiber_path,isHField,scalarName='scalar'):\n");
        file.write("\tfibername=string.rsplit(fiber_path,'.vtk',1)[0]\n");
        file.write("\toutput_dir = out_dir+'/'+ fibername +'/'+ sid\n");
        file.write("\toutput_fiber = output_dir + '/' + sid + '_' + fiber_path\n");
        file.write("\toutput_fvp = string.rsplit(output_fiber,'.vtk',1)[0]+'.fvp'\n\n");

	file.write("\tprint('Running on fiber\case: ' + fibername + '/' + sid)\n");

	file.write("\tif not os.path.exists(fiber_dir+'/'+fiber_path):\n");
        file.write("\t\tprint('Fiber file ' + fiber_path + ' does not exist! Skipping case ' + sid)\n");
        file.write("\t\treturn 0\n");
        file.write("\tif not os.path.exists(scalar_path):\n");
        file.write("\t\tprint('Scalar file ' + scalar_path + ' does not exist! Skipping case ' + sid)\n");
        file.write("\t\treturn 0\n");
        file.write("\tif not os.path.exists(def_path):\n");
        file.write("\t\tprint('Deformation file ' + def_path + ' does not exist! Skipping case ' + sid)\n");
        file.write("\t\treturn 0\n\n");

        file.write("\tif not os.path.exists(out_dir+'/'+fibername):\n");
        file.write("\t\tos.makedirs(out_dir+'/'+fibername)\n");
        file.write("\tif not os.path.exists(output_dir):\n");
        file.write("\t\tos.makedirs(output_dir)\n");
        file.write("\telse:\n");
        file.write("\t\tprint('folder ' + output_dir + ' already exists, skipping...')\n");
	file.write("\t\tif os.path.exists(output_fvp):\n");
        file.write("\t\t\tfvp_results.append((output_fvp,fibername,sid))\n");
        file.write("\t\t\treturn 0\n\n");
     
        file.write("\tif isHField:\n");
        file.write("\t\tcommand = fiberprocess_path + ' -n' + ' --inputFiberBundle '"
                   " + fiber_dir+'/'+fiber_path + ' -o '+ output_fiber + ' -S ' + scalar_path "
                   " + ' -H ' + def_path + ' --scalarName '+scalarName\n");
        file.write("\telse:\n");
        file.write("\t\tcommand = fiberprocess_path + ' -n' + ' --inputFiberBundle '"
                   " + fiber_dir+'/'+fiber_path + ' -o '+ output_fiber + ' -S ' + scalar_path "
                   " + ' -D ' + def_path + ' --scalarName '+scalarName\n");

        file.write("\ttry:\n");
        file.write("\t\ti1 = subprocess.check_call(command,shell=True)\n");
        file.write("\t\tcommand_stat = dti_stat_path + ' --scalarName ' + scalarName + "
                   "' --parameter_list ' + scalarName + ' -o ' + output_fvp + ' -i ' + output_fiber\n");
        file.write("\t\ti2 = subprocess.check_call(command_stat,shell=True)\n");
        file.write("\texcept subprocess.CalledProcessError as e:\n");
        file.write("\t\tprint 'Error! Non-zero status code returned by command: ' + str(e.returncode)\n");
        file.write("\t\tprint e.output\n");
        file.write("\t\ti1=i2=1\n");

        file.write("\t#store fvp information\n");
        file.write("\tif i2==0 :\n");
        file.write("\t\tfvp_results.append((output_fvp,fibername,sid))\n");

        file.write("\treturn i1 + i2 \n\n");

        file.write("#this function contains the procedure to write the final csv\n");
        file.write("def writeCSV(transpose):\n");
        file.write("\tfiber_ref = dict()\n");
        file.write("\tt = time.strftime('%c').replace(' ','-')\n\n");
        file.write("\tfor fvp in fvp_results:\n");
        file.write("\t\tf_table = []\n");
        file.write("\t\tif fvp[1] not in fiber_ref:\n");
        file.write("\t\t\tf_table.append(['Arc_length',fvp[2]])\n");
        file.write("\t\t\tfiber_ref[fvp[1]] = f_table\n");
        file.write("\t\telse:\n");
        file.write("\t\t\tf_table = fiber_ref[fvp[1]]\n");
        file.write("\t\t\tf_table[0].append(fvp[2])\n");
        file.write("\t\tf = open(fvp[0],'r')\n");
        file.write("\t\t# omitting the first five lines\n");
        file.write("\t\tfor i in range(0,5):\n");
        file.write("\t\t\tf.readline()\n");
        file.write("\t\tindex = 1\n");
        file.write("\t\tfor line in f:\n");
        file.write("\t\t\tparameters = line.rstrip().rsplit(',')\n");
        file.write("\t\t\tif len(f_table) > index:\n");
        file.write("\t\t\t\tf_table[index].append(parameters[2])\n");
        file.write("\t\t\telse:\n");
        file.write("\t\t\t\tf_table.append([parameters[0],parameters[2]])\n");
        file.write("\t\t\tindex += 1\n");
        file.write("\t\tf.close()\n\n\n");
        file.write("\tfor (fiber,table) in fiber_ref.items():\n");
        file.write("\t\tif transpose:\n");
        file.write("\t\t\ttable = map(list,zip(*table))\n");
        file.write("\t\tresult = out_dir + '/' + fiber + '/' + 'Result_' + fiber + '_' + t + '.csv'\n");
        file.write("\t\twith open(result,'w') as csvf:\n");
        file.write("\t\t\tcsvw = csv.writer(csvf)\n");
        file.write("\t\t\tcsvw.writerows(table)\n\n\n");

        file.write("# this function contains all the run_process calls\n");
        file.write("def run():\n");
        file.write("\tcode = 0\n");
        for(std::vector<tool::MapData>::iterator it1 = data.begin(); it1 != data.end(); ++it1 )
        {
            for(std::vector<tool::TractData>::iterator it2 = tracts.begin(); it2 != tracts.end(); ++it2)
            {
                char* HF;

                if (isHField)
                    HF = "True";
                else
                    HF = "False";

                char buffer[1024];
                if(scalar_name != "")
                {
                    std::snprintf(buffer,1024,"\tcode += run_process('%s','%s','%s','%s',%s,'%s')\n",
                                  it1->subjectID.toStdString().c_str(),it1->t12_path.toStdString().c_str(),
                                  it1->def_path.toStdString().c_str(), it2->file_path.toStdString().c_str(), HF,
                                  scalar_name.toStdString().c_str());
                }else
                {
                    std::snprintf(buffer,1024,"\tcode += run_process('%s','%s','%s','%s', %s)\n",
                                  it1->subjectID.toStdString().c_str(),it1->t12_path.toStdString().c_str(),
                                  it1->def_path.toStdString().c_str(), it2->file_path.toStdString().c_str(),HF);

                }
                file.write(buffer);
            }
        }
        if(isTranspose)
            file.write("\twriteCSV(True)\n");
        else
            file.write("\twriteCSV(False)\n");
        file.write("\treturn code\n");

        file.write("\nif __name__ == '__main__':\n");
        file.write("\tcode = run()\n");
        file.write("\tif code == 0:\n");
        file.write("\t\tprint(\"Fiber output is successfully generated! Return code = \" + str(code))\n");
        file.write("\telse:\n");
        file.write("\t\tprint('Error occured! Return code =' + str(code))\n");
        file.close();
        ErrorReporter::friendly_fire("Successfully generated script to " + outdir.toStdString() + "/" +pipeline_script.toStdString());
        return true;
    }else
        return false;

}


QString ScriptWriter::getToolScriptName(){
    return tool_script;
}

QString ScriptWriter::getPipelineScriptName(){
    return pipeline_script;
}

void ScriptWriter::close(){

}
